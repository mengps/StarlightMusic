#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "lrcdecoder.h"
#include "lyricsmodel.h"

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
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString singer READ singer NOTIFY singerChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(LyricsModel* lyrics READ lyrics NOTIFY lyricsChanged)
    Q_PROPERTY(int lyricIndex READ lyricIndex NOTIFY lyricIndexChanged)
    Q_PROPERTY(bool running READ running CONSTANT)

public:
    MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    ImageProvider *imageProvider();

    QUrl music() const;
    void setMusic(const QUrl &url);

    int volume() const;
    void setVolume(int vol);

    qreal progress() const;
    void setProgress(qreal progreass);

    qreal duration() const;
    bool running() const;

    QString title() const;
    QString singer() const;
    QString album() const;
    int lyricIndex() const;

    LyricsModel* lyrics() const;

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
    void singerChanged();
    void albumChanged();
    void lyricsChanged();
    void lyricIndexChanged();
    void playbillChanged();

private slots:
    void update();

private:
    bool m_running = false;
    QUrl m_music = QUrl();
    qreal m_progress = 0.0;
    qreal m_duration = 0.0;
    int m_volume = 100;
    QString m_title = QString();
    QString m_singer = QString();
    QString m_album = QString();
    QByteArray m_audioBuffer = QByteArray();
    QTimer *m_playTimer = nullptr;
    QScopedPointer<QAudioOutput> m_audioOutput;
    QIODevice *m_audioDevice = nullptr;
    AudioDecoder *m_decoder = nullptr;

    bool m_hasLyrics = false;
    QScopedPointer<LrcDecoder> m_lrcDecoder;
    LyricsModel *m_lyricsModel = nullptr;
    int m_lyricIndex = 0;
    int m_nextIndex = 0;

    ImageProvider *m_playbillProvider;
};

#endif // MUSICPLAYER_H
