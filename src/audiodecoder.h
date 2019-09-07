#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <QAudioFormat>
#include <QMutex>
#include <QQueue>
#include <QThread>

struct Packet
{
    QByteArray data;
    qreal time;
};

class SwrContext;
class AVFormatContext;
class AVCodecContext;
class AVStream;
class AVPacket;
class AVFrame;
class AudioDecoder : public QThread
{
    Q_OBJECT

public:
    AudioDecoder(QObject *parent = nullptr);
    ~AudioDecoder();

    void stop();
    void open(const QString &filename);
    void setProgress(qreal ratio);

    qreal duration();
    QString title();
    QString lastError();
    QAudioFormat format();
    qreal currentTime();
    QByteArray currentFrame();

signals:
    void error(const QString &err);
    void resolved();

protected:
    void run();

private:
    void resolve();
    void cleanup();

    SwrContext *m_swrContext = nullptr;
    AVFormatContext *m_formatContext = nullptr;
    AVCodecContext *m_codecContext = nullptr;
    AVStream *m_audioStream = nullptr;
    AVPacket *m_packet = nullptr;
    AVFrame *m_frame = nullptr;
    int m_audioIndex = -1;

    qreal m_duration = 0.0;
    qreal m_currentTime = 0.0;
    bool m_runnable = true;
    QAudioFormat m_format;
    QMutex m_mutex;
    QString m_title;
    QString m_filename;
    QQueue<Packet> m_frameQueue;
};

#endif // AUDIODECODER_H
