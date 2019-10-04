#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QUrl>

class ImageProvider;
class LyricsModel;
class MusicData;
class MusicModel;
class MusicPlayerPrivate;
class MusicPlayer : public QObject
{
    Q_OBJECT

    Q_ENUMS(PlayMode)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(PlayMode playMode READ playMode WRITE setPlayMode NOTIFY playModeChanged)
    Q_PROPERTY(MusicData* curMusic READ curMusic WRITE setCurMusic NOTIFY curMusicChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString singer READ singer NOTIFY singerChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(int lyricIndex READ lyricIndex NOTIFY lyricIndexChanged)
    Q_PROPERTY(LyricsModel* lyrics READ lyrics NOTIFY lyricsChanged)
    Q_PROPERTY(MusicModel* music READ music NOTIFY musicChanged)

public:
    enum class PlayMode
    {
        /** @note 顺序播放 */
        Order = 1,
        /** @note 单曲循环 */
        Single,
        /** @note 随机播放 */
        Random
    };

    MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    ImageProvider* imageProvider();

    int volume() const;
    void setVolume(int vol);

    qreal progress() const;
    void setProgress(qreal progreass);

    PlayMode playMode() const;
    void setPlayMode(PlayMode mode);

    MusicData* curMusic() const;
    void setCurMusic(MusicData *music);

    bool playing() const;
    qreal duration() const;
    QString title() const;
    QString singer() const;
    QString album() const;
    int lyricIndex() const;

    LyricsModel* lyrics() const;
    MusicModel* music() const;

    /** @note 开始播放 */
    Q_INVOKABLE void play(const QUrl &url);
    /** @note 暂停播放 */
    Q_INVOKABLE void suspend();
    /** @note 继续播放 */
    Q_INVOKABLE void resume();
    /** @note 播放上一首 */
    Q_INVOKABLE void playPrev();
    /** @note 播放下一首 */
    Q_INVOKABLE void playNext();
    /** @note 添加到播放列表 */
    Q_INVOKABLE void addMusicList(const QList<QUrl> &urls);

signals:
    void error(const QString &errorString);

    void finished();
    void curMusicChanged();
    void progressChanged();
    void volumeChanged();
    void playModeChanged();
    void playingChanged();
    void durationChanged();
    void titleChanged();
    void singerChanged();
    void albumChanged();
    void lyricsChanged();
    void musicChanged();
    void lyricIndexChanged();
    void playbillChanged();

private slots:
    void update();

    void readSettings();
    void writeSettings();

private:
    MusicPlayerPrivate *d = nullptr;
};

#endif // MUSICPLAYER_H
