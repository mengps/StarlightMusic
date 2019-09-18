#include "audiodecoder.h"
#include "bufferqueue.h"

#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QMutex>
#include <QQueue>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#ifdef MKTAG
#undef MKTAG
#define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | (unsigned(d) << 24))
#endif

//无警告版
#ifdef AVERROR_EOF
#undef AVERROR_EOF
#define AVERROR_EOF (-int(MKTAG('E', 'O', 'F', ' ')))
#endif

typedef const char *const_int8ptr;

class AudioDecoderPrivate
{
public:
    AudioDecoderPrivate() { }

    QMutex m_mutex;
    QAudioFormat m_format;
    qreal m_duration = 0.0;
    QString m_title = QString();
    QString m_singer = QString();
    QString m_album = QString();
    QImage m_playbill = QImage();
    QString m_filename = QString();
    QString m_lastError = QString();

    SwrContext *m_swrContext = nullptr;
    AVFormatContext *m_formatContext = nullptr;
    AVCodecContext *m_audioCodecContext = nullptr;
    AVCodecContext *m_videoCodecContext = nullptr;
    AVStream *m_audioStream = nullptr;
    AVStream *m_videoStream = nullptr;
    AVPacket *m_packet = nullptr;
    AVFrame *m_frame = nullptr;
    int m_audioIndex = -1;
    int m_videoIndex = -1;
    bool m_runnable = true;

    //填充3字节,去除警告
    char paddingByte[7];

    //缓冲队列
    BufferQueue<AudioPacket> m_bufferQueue;

    bool openCodecContext(AVMediaType type, AVFormatContext *&formatCtx, AVCodecContext * &codecCtx, int *stream_index);

    AVSampleFormat converSampleFormat(QAudioFormat::SampleType format)
    {
        AVSampleFormat type;
        switch (format) {
            case QAudioFormat::Float:
            type = AV_SAMPLE_FMT_FLT;
            break;

            default:
            case QAudioFormat::SignedInt:
            type = AV_SAMPLE_FMT_S32;
            break;
        }

        return type;
    }

    bool resolve();
    void cleanup();
};


bool AudioDecoderPrivate::resolve()
{
    //打开输入文件,并分配格式上下文
    avformat_open_input(&m_formatContext, m_filename.toStdString().c_str(), nullptr, nullptr);

    if (!m_formatContext) {
        m_lastError = "未知的音乐格式.";
        return false;
    }

    avformat_find_stream_info(m_formatContext, nullptr);

    if (!openCodecContext(AVMEDIA_TYPE_AUDIO, m_formatContext, m_audioCodecContext, &m_audioIndex))
        return false;

    if (!openCodecContext(AVMEDIA_TYPE_VIDEO, m_formatContext, m_videoCodecContext, &m_videoIndex))
        m_lastError = "无法找到或解析海报.";

    m_audioStream = m_formatContext->streams[m_audioIndex];
    m_videoStream = m_formatContext->streams[m_videoIndex];

    //打印相关信息
    av_dump_format(m_formatContext, 0, "audio", 0);

    QAudioFormat format;
    format.setCodec("audio/pcm");
    format.setSampleRate(m_audioCodecContext->sample_rate);
    format.setChannelCount(m_audioCodecContext->channels);
    if (m_audioCodecContext->sample_fmt == AV_SAMPLE_FMT_FLT) {
        format.setSampleType(QAudioFormat::Float);
        format.setSampleSize(8 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT));
    }else {
        format.setSampleType(QAudioFormat::SignedInt);
        format.setSampleSize(8 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S32));
    }
    m_format = format;
    m_duration = m_audioStream->duration * av_q2d(m_audioStream->time_base);

    AVDictionaryEntry *title = av_dict_get(m_formatContext->metadata, "title", nullptr, AV_DICT_MATCH_CASE);
    AVDictionaryEntry *artist = av_dict_get(m_formatContext->metadata, "artist", nullptr, AV_DICT_MATCH_CASE);
    AVDictionaryEntry *album = av_dict_get(m_formatContext->metadata, "album", nullptr, AV_DICT_MATCH_CASE);
    if (album) m_album = album->value;
    if (artist) m_singer = artist->value;
    if (title) m_title = title->value;
    else m_title = QFileInfo(m_filename).baseName();

    m_swrContext = swr_alloc_set_opts(nullptr, int64_t(m_audioCodecContext->channel_layout), converSampleFormat(m_format.sampleType()),
                                      m_audioCodecContext->sample_rate, int64_t(m_audioCodecContext->channel_layout),
                                      m_audioCodecContext->sample_fmt, m_audioCodecContext->sample_rate, 0, nullptr);
    swr_init(m_swrContext);

    //分配并初始化一个临时的帧和包
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_packet->data = nullptr;
    m_packet->size = 0;

    return true;
}

void AudioDecoderPrivate::cleanup()
{
    m_audioIndex = m_videoIndex = -1;
    m_audioStream = m_videoStream = nullptr;
    m_title.clear();
    m_singer = QString("未知");
    m_album =  QString("无");
    m_playbill = QImage();
    m_duration = 0.0;
    m_runnable = true;
    m_lastError.clear();

    if (m_frame) av_frame_free(&m_frame);
    if (m_packet) av_packet_free(&m_packet);
    if (m_swrContext) swr_free(&m_swrContext);
    if (m_audioCodecContext) avcodec_free_context(&m_audioCodecContext);
    if (m_videoCodecContext) avcodec_free_context(&m_videoCodecContext);
    if (m_formatContext) avformat_close_input(&m_formatContext);
}

AudioDecoder::AudioDecoder(QObject *parent)
    : QThread (parent)
{
    d = new AudioDecoderPrivate;
}

AudioDecoder::~AudioDecoder()
{
    stop();
    d->cleanup();
    delete d;
}

void AudioDecoder::stop()
{
    //必须先重置信号量
    d->m_bufferQueue.init();
    d->m_runnable = false;
    wait();
}

void AudioDecoder::open(const QString &filename)
{
    stop();

    d->m_mutex.lock();
    d->m_filename = filename;
    d->cleanup();
    bool success = d->resolve();
    d->m_mutex.unlock();

    if (!success) {
        emit error(d->m_lastError);
        return;
    } else {
        start();
        emit resolved();
    }
}

void AudioDecoder::setProgress(qreal ratio)
{
    d->m_mutex.lock();
    qreal seconds = ratio * d->m_duration;
    av_seek_frame(d->m_formatContext, d->m_audioIndex, int64_t(seconds * d->m_audioStream->time_base.den), AVSEEK_FLAG_BACKWARD);
    d->m_mutex.unlock();
    d->m_bufferQueue.init();
    //@warning 因为有maxQueueSize控制缓冲,因此会出现这种情况
    //解码结束但播放未结束,需要重新启动解码线程
    if (!isRunning()) start();
}

QAudioFormat AudioDecoder::format()
{
    QMutexLocker locker(&d->m_mutex);
    return d->m_format;
}

qreal AudioDecoder::duration()
{
    QMutexLocker locker(&d->m_mutex);
    return d->m_duration;
}

QString AudioDecoder::title()
{
    QMutexLocker locker(&d->m_mutex);
    return d->m_title;
}

QString AudioDecoder::singer()
{
    QMutexLocker locker(&d->m_mutex);
    return d->m_singer;
}

QString AudioDecoder::album()
{
    QMutexLocker locker(&d->m_mutex);
    return d->m_album;
}

AudioPacket AudioDecoder::currentPacket()
{
    AudioPacket packet;
    //可以考虑用tryDequeue()代替
    packet = d->m_bufferQueue.tryDequeue();

    return packet;
}

void AudioDecoder::run()
{
    //读取下一帧
    AVSampleFormat fmt = d->converSampleFormat(d->m_format.sampleType());
    while (d->m_runnable && (av_read_frame(d->m_formatContext, d->m_packet) >= 0)) {

        if (d->m_packet->stream_index == d->m_audioIndex) {
            //发送给解码器
            int ret = avcodec_send_packet(d->m_audioCodecContext, d->m_packet);

            QByteArray data;
            while (ret >= 0) {
                //从解码器接收解码后的帧
                ret = avcodec_receive_frame(d->m_audioCodecContext, d->m_frame);

                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
                else if (ret < 0) return;

                int size = av_samples_get_buffer_size(nullptr, d->m_frame->channels, d->m_frame->nb_samples, fmt, 0);
                uint8_t *buf = new uint8_t[size];
                swr_convert(d->m_swrContext, &buf, d->m_frame->nb_samples, const_cast<const uint8_t**>(d->m_frame->data), d->m_frame->nb_samples);
                data += QByteArray(const_int8ptr(buf), size);
                delete[] buf;

                qreal time = d->m_frame->pts * av_q2d(d->m_audioStream->time_base) + d->m_frame->pkt_duration * av_q2d(d->m_audioStream->time_base);

                d->m_bufferQueue.enqueue({ data, time });

                av_frame_unref(d->m_frame);
            }
        } else if (d->m_packet->stream_index == d->m_videoIndex && d->m_playbill.isNull()/*只要一帧*/) {
            //发送给解码器
            int ret = avcodec_send_packet(d->m_videoCodecContext, d->m_packet);

            while (ret >= 0) {
                //从解码器接收解码后的帧
                ret = avcodec_receive_frame(d->m_videoCodecContext, d->m_frame);

                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
                else if (ret < 0) return;

                int dst_linesize[4];
                uint8_t *dst_data[4];
                SwsContext *swsContext = sws_getContext(d->m_frame->width, d->m_frame->height, d->m_videoCodecContext->pix_fmt,d-> m_frame->width,
                                                        d->m_frame->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
                av_image_alloc(dst_data, dst_linesize, d->m_frame->width, d->m_frame->height, AV_PIX_FMT_RGB24, 1);
                sws_scale(swsContext, d->m_frame->data, d->m_frame->linesize, 0, d->m_frame->height, dst_data, dst_linesize);

                //注意后面的copy(),dst_data[0]释放后image也无效了,因此必须拷贝一份
                QImage image = QImage(dst_data[0], d->m_frame->width,d-> m_frame->height, dst_linesize[0], QImage::Format_RGB888).copy();
                av_freep(&dst_data[0]);
                sws_freeContext(swsContext);

                //m_playbill只在内部使用
                d->m_playbill = image;
                emit hasPlaybill(image);

                av_frame_unref(d->m_frame);
            }
        }

        av_packet_unref(d->m_packet);
    }
}

bool AudioDecoderPrivate::openCodecContext(AVMediaType type, AVFormatContext * &formatCtx, AVCodecContext * &codecCtx, int *stream_index)
{
    //找到流的索引
    int ret = av_find_best_stream(formatCtx, type, -1, -1, nullptr, 0);
    const char *typeStr = av_get_media_type_string(type);

    if (ret < 0) {
        m_lastError = QString("无法找到%1的%2流'").arg(m_filename).arg(typeStr);
        return false;
    }  else {
        *stream_index = ret;
        AVStream *stream = formatCtx->streams[ret];
        AVCodec *decoder = nullptr;

        if (stream) decoder = avcodec_find_decoder(stream->codecpar->codec_id);

        if (!decoder) {
            m_lastError = QString("无法找到%1编解码器").arg(typeStr);
            return false;
        }

        codecCtx = avcodec_alloc_context3(decoder);
        if (!codecCtx) {
           m_lastError = QString("无法分配%1编解码上下文").arg(typeStr);
            return false;
        }

        int ret = avcodec_parameters_to_context(codecCtx, stream->codecpar);
        if (ret < 0) {
            m_lastError = QString("无法将%1编解码器参数复制到解码器上下文").arg(typeStr);
            return false;
        }

        ret = avcodec_open2(codecCtx, decoder, nullptr);
        if (ret < 0) {
            m_lastError = QString("打开%1编解码器失败").arg(typeStr);
            return false;
        }
    }

    return true;
}

