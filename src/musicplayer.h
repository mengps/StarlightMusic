#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QUrl>

class AudioDecoder;
class ImageProvider;
class QAudioOutput;
class QIODevice;
class QTimer;
class MusicPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl music READ music WRITE setMusic NOTIFY musicChanged)
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString author READ author NOTIFY authorChanged)
    Q_PROPERTY(bool running READ running CONSTANT)

public:
    MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    ImageProvider *imageProvider();

    QUrl music() const;
    void setMusic(const QUrl &url);

    qreal progress() const;
    void setProgress(qreal progreass);

    int volume() const;
    void setVolume(int vol);

    qreal duration() const;
    bool running() const;

    QString title() const;
    QString author() const;
    QString album() const;

    /** @note 开始播放 */
    Q_INVOKABLE void play(const QUrl &url);
    /** @note 暂停播放 */
    Q_INVOKABLE void suspend();
    /** @note 继续播放 */
    Q_INVOKABLE void resume();

signals:
    void error(const QString &errorString);

    void finished();
    void musicChanged();
    void progressChanged();
    void volumeChanged();
    void durationChanged();
    void titleChanged();
    void authorChanged();
    void playbillChanged();

private slots:
    void update();

private:
    ImageProvider *m_playbillProvider;

    bool m_running = false;
    QUrl m_music = QUrl();
    qreal m_progress = 0.0;
    int m_volume = 100;
    QByteArray m_audioBuffer = QByteArray();
    QTimer *m_playTimer = nullptr;
    QScopedPointer<QAudioOutput> m_audioOutput;
    QIODevice *m_audioDevice = nullptr;
    AudioDecoder *m_decoder = nullptr;
};

#endif // MUSICPLAYER_H
