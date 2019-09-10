#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <QAudioFormat>
#include <QThread>

typedef struct AudioPacket
{
    QByteArray data;
    qreal time;
} AudioPacket;

class AudioDecoderPrivate;
class AudioDecoder : public QThread
{
    Q_OBJECT

public:
    AudioDecoder(QObject *parent = nullptr);
    ~AudioDecoder();

    /**
     * @note is thread-safe
     * @brief 通知解码器停止解码
     * @warning 可能不会立即停止
     */
    void stop();

    /**
     * @note is thread-safe
     * @param filename 媒体文件名(通常是音乐)
     */
    void open(const QString &filename);

    /**
     * @note is thread-safe
     * @param ratio 进度的比率,在 0.0 ~ 1.0 之间
     */
    void setProgress(qreal ratio);

    /**
     * @note is thread-safe
     * @return 持续时间(总时长)
     */
    qreal duration();

    /**
     * @note is thread-safe
     * @return 标题,如果没有,则为文件名(无后缀)
     */
    QString title();

    /**
     * @note is thread-safe
     * @return 歌手,如果没有,则为'未知'
     */
    QString singer();

    /**
     * @note is thread-safe
     * @return 专辑,如果没有,则为'无'
     */
    QString album();

    /**
     * @note is thread-safe
     * @return 音频的格式
     */
    QAudioFormat format();

    /**
     * @note is thread-safe
     * @return 解码后的音频包{data + time}
     */
    AudioPacket currentPacket();

signals:
    /**
     * @note 当出现错误时发出
     * @param error 错误信息
     */
    void error(const QString &error);

    /**
     * @note 文件解析完成后发出
     * @warning 此信号在解复用后 -> 解码前发出
     */
    void resolved();

    /**
     * @warning 对于音乐,海报在视频流中,需要通过解码获得
     *          因此,当解码出海报后以信号形式通知
     * @param playbill QImage形式的海报
     */
    void hasPlaybill(const QImage &playbill);

protected:
    void run();

private:
    AudioDecoderPrivate *d = nullptr;
};

#endif // AUDIODECODER_H
