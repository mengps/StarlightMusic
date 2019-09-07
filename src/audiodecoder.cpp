#include "audiodecoder.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}

#include <QDebug>
#include <QFileInfo>
#include <QSemaphore>

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

//         -1               +1
//   [free space] -> [useable space]
static const int maxQueueSize = 100;
static QSemaphore freeSpace(maxQueueSize);
static QSemaphore useableSpace(0);

static void semaphoreInit()
{
    if (useableSpace.available() > 0)
        useableSpace.acquire(useableSpace.available());
    if (freeSpace.available() < maxQueueSize)
        freeSpace.release(maxQueueSize - freeSpace.available());
}

AudioDecoder::AudioDecoder(QObject *parent)
    : QThread (parent)
{

}

AudioDecoder::~AudioDecoder()
{

}

void AudioDecoder::stop()
{
    //必须先重置信号量
    semaphoreInit();
    m_mutex.lock();
    m_frameQueue.clear();
    m_runnable = false;
    m_mutex.unlock();
    wait();
}

void AudioDecoder::open(const QString &filename)
{
    semaphoreInit();
    m_mutex.lock();
    m_filename = filename;
    cleanup();
    resolve();
    m_mutex.unlock();
    start();
}

void AudioDecoder::setProgress(qreal ratio)
{
    //@warnning 因为有maxQueueSize控制缓冲,因此会出现这种情况
    //解码结束但播放未结束,需要重新启动解码线程
    if (!isRunning()) start();
    m_mutex.lock();
    qreal seconds = ratio * m_duration;
    m_currentTime = seconds;
    av_seek_frame(m_formatContext, m_audioIndex, int64_t(seconds * m_audioStream->time_base.den), AVSEEK_FLAG_ANY);
    m_frameQueue.clear();
    m_mutex.unlock();
    semaphoreInit();
}

QAudioFormat AudioDecoder::format()
{
    QMutexLocker locker(&m_mutex);
    return m_format;
}

qreal AudioDecoder::duration()
{
    QMutexLocker locker(&m_mutex);
    return m_duration;
}

QString AudioDecoder::title()
{
    QMutexLocker locker(&m_mutex);
    return m_title;
}

qreal AudioDecoder::currentTime()
{
    QMutexLocker locker(&m_mutex);
    return m_currentTime;
}

QByteArray AudioDecoder::currentFrame()
{
    QByteArray data = QByteArray();
    if (!m_frameQueue.isEmpty()) {
        useableSpace.acquire();
        Packet packet = m_frameQueue.dequeue();
        freeSpace.release();
        data += packet.data;
        m_currentTime = packet.time;
    }

    return data;
}

void AudioDecoder::run()
{
    emit resolved();
    //读取下一帧
    while (m_runnable && av_read_frame(m_formatContext, m_packet) >= 0) {
        if (m_packet->stream_index == m_audioIndex) {
            //发送给解码器
            int ret = avcodec_send_packet(m_codecContext, m_packet);

            QByteArray data;
            while (ret >= 0) {
                //从解码器接收解码后的帧
                ret = avcodec_receive_frame(m_codecContext, m_frame);

                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
                else if (ret < 0) return;

                int size = av_samples_get_buffer_size(nullptr, m_frame->channels, m_frame->nb_samples, AV_SAMPLE_FMT_S32, 0);
                uint8_t *buf = new uint8_t[size];
                swr_convert(m_swrContext, &buf, m_frame->nb_samples, const_cast<const uint8_t**>(m_frame->data), m_frame->nb_samples);
                data += QByteArray(const_int8ptr(buf), size);
                delete[] buf;

                qreal time = m_frame->pts * av_q2d(m_audioStream->time_base) + m_frame->pkt_duration * av_q2d(m_audioStream->time_base);

                freeSpace.acquire();
                m_frameQueue.enqueue({ data, time });
                //相当于useableSpace.release();
                QSemaphoreReleaser releaser(useableSpace);

                av_frame_unref(m_frame);
            }
        }

        av_packet_unref(m_packet);
    }
}

void AudioDecoder::resolve()
{
    //打开输入文件，并分配格式上下文
    avformat_open_input(&m_formatContext, m_filename.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(m_formatContext, nullptr);

    //找到视频流的索引
    m_audioIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    const char *typeStr = av_get_media_type_string(AVMEDIA_TYPE_AUDIO);

    if (m_audioIndex < 0) {
        emit error(QString("无法找到%1的%2流'").arg(m_filename).arg(typeStr));
        return;
    }  else {
        m_audioStream = m_formatContext->streams[m_audioIndex];
        AVCodec *audioDecoder = nullptr;

        if (m_audioStream)
            audioDecoder = avcodec_find_decoder(m_audioStream->codecpar->codec_id);

        if (!audioDecoder) {
            emit error( QString("无法找到%1编解码器").arg(typeStr));
            return;
        }

        m_codecContext = avcodec_alloc_context3(audioDecoder);
        if (!m_codecContext) {
            emit error(QString("无法分配%1编解码上下文").arg(typeStr));
            return;
        }

        int ret = avcodec_parameters_to_context(m_codecContext, m_audioStream->codecpar);
        if (ret < 0) {
            emit error(QString("无法将％1编解码器参数复制到解码器上下文").arg(typeStr));
            return;
        }

        ret = avcodec_open2(m_codecContext, audioDecoder, nullptr);
        if (ret < 0) {
            emit error(QString("打开%1编解码器失败").arg(typeStr));
            return;
        }
    }

    //打印相关信息
    //av_dump_format(m_formatContext, 0, "format", 0);
    //fflush(stderr);

    QAudioFormat format;
    format.setCodec("audio/pcm");
    format.setSampleRate(m_codecContext->sample_rate);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(8 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S32));
    format.setChannelCount(m_codecContext->channels);
    m_format = format;
    m_duration = m_audioStream->duration * av_q2d(m_audioStream->time_base);
    AVDictionaryEntry *next = av_dict_get(m_formatContext->metadata, "title", nullptr, AV_DICT_MATCH_CASE);
    if (next) m_title = next->value;
    else m_title = QFileInfo(m_filename).baseName();

    m_swrContext = swr_alloc_set_opts(nullptr, int64_t(m_codecContext->channel_layout), AV_SAMPLE_FMT_S32, m_codecContext->sample_rate,
                                    int64_t(m_codecContext->channel_layout), m_codecContext->sample_fmt, m_codecContext->sample_rate,
                                    0, nullptr);
    swr_init(m_swrContext);

    //分配并初始化一个临时的帧和包
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_packet->data = nullptr;
    m_packet->size = 0;
}

void AudioDecoder::cleanup()
{
    m_audioIndex = -1;
    m_audioStream = nullptr;
    m_title.clear();
    m_duration = 0.0;
    m_currentTime = 0.0;
    m_runnable = true;
    m_frameQueue.clear();

    if (m_frame) av_frame_free(&m_frame);
    if (m_packet) av_packet_free(&m_packet);
    if (m_swrContext) swr_free(&m_swrContext);
    if (m_codecContext) avcodec_free_context(&m_codecContext);
    if (m_formatContext) avformat_close_input(&m_formatContext);
}
