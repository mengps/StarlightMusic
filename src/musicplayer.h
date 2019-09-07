#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QUrl>

class AudioDecoder;
class QAudioOutput;
class QIODevice;
class QTimer;
class MusicPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl music READ music WRITE setMusic NOTIFY musicChanged)
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool running READ running CONSTANT)

public:
    MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    QUrl music() const;
    void setMusic(const QUrl &url);

    qreal progress() const;
    void setProgress(qreal progreass);

    qreal duration() const;
    bool running() const;
    QString title() const;

    //开始播放
    Q_INVOKABLE void play(const QUrl &url);
    //暂停播放
    Q_INVOKABLE void suspend();
    //继续播放
    Q_INVOKABLE void resume();

signals:
    void finished();
    void error(const QString &errorString);
    void musicChanged();
    void progressChanged();
    void durationChanged();
    void titleChanged();

private slots:
    void update();

private:
    bool m_running = false;
    QUrl m_music;
    qreal m_progress;
    QByteArray m_audioBuffer;
    QTimer *m_playTimer;
    QScopedPointer<QAudioOutput> m_audioOutput;
    QIODevice *m_audioDevice = nullptr;
    AudioDecoder *m_decoder = nullptr;
};

#endif // MUSICPLAYER_H
