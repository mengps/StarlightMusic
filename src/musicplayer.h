#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "lrcdecoder.h"
#include "lyricsmodel.h"
#include "musicmodel.h"

#include <QObject>
#include <QUrl>

class ImageProvider;
class MusicPlayerPrivate;
class MusicPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl curMusic READ curMusic WRITE setCurMusic NOTIFY curMusicChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString singer READ singer NOTIFY singerChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(int lyricIndex READ lyricIndex NOTIFY lyricIndexChanged)
    Q_PROPERTY(LyricsModel* lyrics READ lyrics NOTIFY lyricsChanged)
    Q_PROPERTY(MusicModel* music READ music NOTIFY musicChanged)
    Q_PROPERTY(bool running READ running CONSTANT)

public:
    MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    ImageProvider* imageProvider();

    QUrl curMusic() const;
    void setCurMusic(const QUrl &url);

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
    MusicModel* music() const;

    /** @note 开始播放 */
    Q_INVOKABLE void play(const QUrl &url);
    /** @note 暂停播放 */
    Q_INVOKABLE void suspend();
    /** @note 继续播放 */
    Q_INVOKABLE void resume();
    /** @note 添加到播放列表 */
    Q_INVOKABLE void addMusicList(const QList<QUrl> &urls);

signals:
    void error(const QString &errorString);

    void finished();
    void curMusicChanged();
    void progressChanged();
    void volumeChanged();
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

private:
    MusicPlayerPrivate *d = nullptr;
};

#endif // MUSICPLAYER_H
