#include "audiodecoder.h"
#include "imageprovider.h"
#include "lrcdecoder.h"
#include "lyricsmodel.h"
#include "musicplayer.h"
#include "musicmodel.h"
#include "skinmanager.h"

#include <QDebug>
#include <QAudioOutput>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QHash>
#include <QSettings>
#include <QTimer>
#include <QUrl>

/**
 * @note ffmpeg所有可打开的字幕格式。
 *       因此，需要去掉它们。
 *       剩下的即为支持的音频和视频格式。
 */
static QHash<QString, bool> SubtitleFormat =
{
    {"srt", true}, {"SRT", true}, {"ssa", true},  {"SSA", true},
    {"ass", true}, {"ASS", true}, {"txt", true},  {"TXT", true},
    {"lrc", true}, {"LRC", true}, {"sup", true},  {"SUP", true},
    {"stl", true}, {"STL", true}, {"aqt", true},  {"AQT", true},
    {"smi", true}, {"SMI", true}, {"pjs", true},  {"PJS", true},
    {"rt", true},  {"RT", true},  {"sami", true}, {"SAMI", true}
};

class MusicPlayerPrivate
{
public:
    bool m_playing = false;
    bool m_decoding = false;
    MusicPlayer::PlayMode m_playMode = MusicPlayer::PlayMode::Order;
    AudioData *m_curMusic = nullptr;
    QString m_skinName = QString();
    qreal m_progress = 0.0;
    int m_volume = 80;
    QByteArray m_audioBuffer = QByteArray();
    QTimer *m_playTimer = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QIODevice *m_audioDevice = nullptr;
    AudioDecoder *m_decoder = nullptr;

    bool m_hasLyrics = false;
    QScopedPointer<LrcDecoder> m_lrcDecoder;
    LyricsModel *m_lyricsModel = nullptr;
    MusicModel *m_musicModel = nullptr;
    int m_lyricIndex = 0;
    int m_nextIndex = 0;
    /** @note 用于去重 */
    QHash<QString, bool> m_files;
    QSettings *m_settings = nullptr;
    ImageProvider *m_playbillProvider;

    bool contains(const QString &filename);
    void loadLyrics(const QString &filename);
};

bool MusicPlayerPrivate::contains(const QString &filename)
{
    if (m_files.value(filename)) {
        return true;
    } else {
        m_files[filename] = true;
        return false;
    }
}

void MusicPlayerPrivate::loadLyrics(const QString &lrcFile)
{
    if (QFileInfo::exists(lrcFile)) {
        if (m_lrcDecoder->decode(lrcFile.toLocal8Bit().data())) {
            //创建Model
            QVector<LyricData *> model;
            LyricPacket packet = m_lrcDecoder->readPacket();
            while (!packet.isEmpty()) {
                LyricData *data = new LyricData(QString::fromStdString(packet.lyric), packet.pts);
                model.append(data);
                packet = m_lrcDecoder->readPacket();
            }
            if (model.count() > 0) {
                m_lyricsModel->setModel(model);
                m_hasLyrics = true;
                if ((m_nextIndex + 1) < m_lyricsModel->count()) m_nextIndex++;
                //打印LRC元数据
                m_lrcDecoder->dumpMetadata(stdout);
            }
        }
    }
}

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject (parent)
{
    qsrand(uint(time(nullptr)));
    d = new MusicPlayerPrivate;

    d->m_playbillProvider = new ImageProvider;
    d->m_playTimer = new QTimer(this);
    connect(d->m_playTimer, &QTimer::timeout, this, &MusicPlayer::update);

    d->m_lrcDecoder.reset(new LrcDecoder);

    d->m_lyricsModel = new LyricsModel(this);
    d->m_musicModel = new MusicModel(this);
    d->m_decoder = new AudioDecoder(this);
    connect(d->m_decoder, &AudioDecoder::error, this, &MusicPlayer::error);
    connect(d->m_decoder, &AudioDecoder::hasPlaybill, this, [this](const QImage &playbill) {
        d->m_playbillProvider->setImage(playbill);
        emit playbillChanged();
    });
    connect(d->m_decoder, &AudioDecoder::resolved, this, [this]() {
        d->m_playing = true;
        d->m_decoding = true;        
        emit playingChanged();

        auto format = d->m_decoder->format();
        d->m_audioOutput->deleteLater();
        d->m_audioOutput = new QAudioOutput(d->m_decoder->format());
        //发现SQ品质码率较高，导致缓冲区不够
        d->m_audioOutput->setBufferSize(format.sampleRate() * format.sampleSize() / 8);
        d->m_audioDevice = d->m_audioOutput->start();
        d->m_audioOutput->setVolume(d->m_volume / qreal(100.0));
        d->m_playTimer->start(100);
    });

    d->m_settings = new QSettings(qApp->applicationDirPath() + "/Settings/settings.ini",
                                  QSettings::IniFormat, this);
    readSettings();
}

MusicPlayer::~MusicPlayer()
{
    suspend();
    writeSettings();
    delete d;
}

ImageProvider *MusicPlayer::imageProvider()
{
    return d->m_playbillProvider;
}

qreal MusicPlayer::progress() const
{
    return d->m_progress;
}

void MusicPlayer::setProgress(qreal ratio)
{
    if (d->m_decoding && !qFuzzyIsNull(qAbs(ratio - d->m_progress))) {
        if (qFuzzyCompare(d->m_progress, 1.0000000)){
            emit finished();
        }
        d->m_progress = ratio;
        d->m_audioBuffer.clear();
        emit progressChanged();
        d->m_decoder->setProgress(ratio);
        if (d->m_hasLyrics) {
            int64_t pts = int64_t(ratio * duration() * 1000);
            int count = d->m_lyricsModel->count();
            for (int i = 0; i < count; i++) {
                if (d->m_lyricsModel->at(i)->pts() > pts) {
                    d->m_lyricIndex = (i > 0) ? (i - 1) : 0;
                    d->m_nextIndex = (d->m_lyricIndex + 1) < count ? d->m_lyricIndex + 1 : d->m_lyricIndex;
                    emit lyricIndexChanged();
                    break;
                }
            }
        }
    }
}

MusicPlayer::PlayMode MusicPlayer::playMode() const
{
    return d->m_playMode;
}

void MusicPlayer::setPlayMode(MusicPlayer::PlayMode mode)
{
    if (mode != d->m_playMode) {
        d->m_playMode = mode;
        emit playModeChanged();
    }
}

bool MusicPlayer::playing() const
{
    return d->m_playing;
}

int MusicPlayer::volume() const
{
    return d->m_volume;
}

void MusicPlayer::setVolume(int vol)
{
    if (vol != d->m_volume) {
        d->m_volume = vol;
        if (d->m_audioOutput) {
            d->m_audioOutput->setVolume(d->m_volume / qreal(100.0));
        }
        emit volumeChanged();
    }
}

qreal MusicPlayer::duration() const
{
    if (d->m_curMusic) return d->m_curMusic->duration();
    else return 0.0;
}

QString MusicPlayer::title() const
{
    if (d->m_curMusic) return d->m_curMusic->title();
    else return QString();
}

QString MusicPlayer::singer() const
{
    if (d->m_curMusic) return d->m_curMusic->singer();
    else return QString();
}

QString MusicPlayer::album() const
{
    if (d->m_curMusic) return d->m_curMusic->album();
    else return QString();
}

int MusicPlayer::lyricIndex() const
{
    return d->m_lyricIndex;
}

LyricsModel* MusicPlayer::lyrics() const
{
    return d->m_lyricsModel;
}

MusicModel* MusicPlayer::music() const
{
    return d->m_musicModel;
}

AudioData* MusicPlayer::curMusic() const
{
    return d->m_curMusic;
}

void MusicPlayer::setCurMusic(AudioData *music)
{
    if (d->m_curMusic != music) {
        d->m_curMusic = music;
        emit curMusicChanged();
        emit titleChanged();
        emit singerChanged();
        emit albumChanged();
        emit durationChanged();
    }
}

QString MusicPlayer::skinName() const
{
    return d->m_skinName;
}

void MusicPlayer::setSkinName(const QString &name)
{
    if (name != d->m_skinName) {
        if (SkinManager::instance()->loadSkin(name)){
            d->m_skinName = name;
            emit skinNameChanged();
        }
    }
}

void MusicPlayer::play(const QUrl &url)
{
    suspend();
    d->m_playing = false;
    d->m_decoding = false;
    d->m_progress = 0.0;
    d->m_audioBuffer.clear();
    d->m_hasLyrics = false;
    d->m_playbillProvider->setImage(QImage(":/image/music.png"));
    emit progressChanged();
    emit playbillChanged();

    QString filename = url.toLocalFile();
    d->m_decoder->open(filename);

    d->m_lyricsModel->clear();
    d->m_lyricIndex = 0;
    d->m_nextIndex = 0;
    int suffixLength = QFileInfo(filename).suffix().length();
    QString lrcFile = filename.mid(0, filename.length() - suffixLength - 1) + ".lrc";
    d->loadLyrics(lrcFile);
}

void MusicPlayer::suspend()
{
    if(d->m_playTimer->isActive()) {
        d->m_playTimer->stop();
        d->m_playing = false;
        emit playingChanged();
    }
}

void MusicPlayer::resume()
{
    if (d->m_decoding) {
        d->m_playTimer->start(100);
        d->m_playing = true;
        emit playingChanged();
    } else if (d->m_curMusic) {
        play(d->m_curMusic->filename());
    }
}

void MusicPlayer::playPrev()
{
    int index = d->m_musicModel->indexof(d->m_curMusic);

    if (index == -1) return;

    switch (d->m_playMode) {
    case PlayMode::Order:
        if (index == 0) {
            index = d->m_musicModel->count() - 1;
        } else {
            index--;
        }
        break;
    case PlayMode::Single:
        break;
    case PlayMode::Random:
        index = qrand() % d->m_musicModel->count();
        break;
    }

    AudioData *music = d->m_musicModel->at(index);
    setCurMusic(music);
    play(music->filename());
}

void MusicPlayer::playNext()
{
    int index = d->m_musicModel->indexof(d->m_curMusic);

    if (index == -1) return;

    switch (d->m_playMode) {
    case PlayMode::Order:
        if (index == (d->m_musicModel->count() - 1)) {
            index = 0;
        } else {
            index++;
        }
        break;
    case PlayMode::Single:
        break;
    case PlayMode::Random:
        index = qrand() % d->m_musicModel->count();
        break;
    }

    AudioData *music = d->m_musicModel->at(index);
    setCurMusic(music);
    play(music->filename());
}

void MusicPlayer::addMusicList(const QList<QUrl> &urls)
{
    bool init = false;
    for (auto url: urls) {
        QString filename = url.toLocalFile();
        if (!SubtitleFormat.value(QFileInfo(filename).suffix())) {
            if (d->contains(filename)) {
                continue;
            } else {
                AudioData *data = new AudioData(url, this);
                connect(data, &AudioData::created, this, [&init, this, data] {
                    if (!init) {
                        init = true;
                        setCurMusic(data);
                        play(data->filename());
                    }
                    d->m_musicModel->append(data);
                    emit d->m_musicModel->modelChanged();
                });
                data->create();
            }
        }
    }
}

void MusicPlayer::update()
{
    while (d->m_audioBuffer.size() < d->m_audioOutput->bytesFree()) {
        AudioPacket packet = d->m_decoder->currentPacket();
        QByteArray data = packet.data;
        qreal currentTime = packet.time;
        if (currentTime >= duration() || (data.isEmpty() && qFuzzyIsNull(currentTime))) {
            d->m_progress = 1.0;
            d->m_playing = false;
            d->m_decoding = false;
            d->m_decoder->stop();
            d->m_playTimer->stop();
            emit finished();
            emit playingChanged();
            emit progressChanged();
            return;
        } else {
            if (d->m_hasLyrics) {
                int64_t pts = int64_t(currentTime) * 1000;
                if (pts > d->m_lyricsModel->at(d->m_lyricIndex)->pts() && pts > d->m_lyricsModel->at(d->m_nextIndex)->pts()) {
                    d->m_lyricIndex = d->m_nextIndex;
                    if ((d->m_nextIndex + 1) < d->m_lyricsModel->count()) d->m_nextIndex++;
                    emit lyricIndexChanged();
                }
            }
            d->m_progress = currentTime / duration();
            emit progressChanged();
        }

        if (data.isEmpty()) break;
        d->m_audioBuffer += data;
    }

    int readSize = d->m_audioOutput->periodSize();
    int chunks = d->m_audioBuffer.size() / readSize;
    while (chunks--) {
        QByteArray pcm = d->m_audioBuffer.mid(0, readSize);
        int size = pcm.size();
        d->m_audioBuffer.remove(0, size);

        if (size) d->m_audioDevice->write(pcm);
        if (size != readSize) break;
    }
}

void MusicPlayer::readSettings()
{
    if (d->m_settings->status() == QSettings::NoError) {
        d->m_settings->beginGroup("MusicPlayer");
        setSkinName(d->m_settings->value("SkinName", ":/skin/default_black.skin").toString());
        QUrl curMusic = d->m_settings->value("CurMusic").toUrl();
        setPlayMode(PlayMode(d->m_settings->value("PlayMode").toInt()));
        int size = d->m_settings->beginReadArray("MusicList");
        QList<QUrl> musiclist;
        for (int i = 0; i < size; i++) {
           d->m_settings->setArrayIndex(i);
           musiclist << d->m_settings->value("filename").toUrl();
        }
        for (auto url: musiclist) {
            QString filename = url.toLocalFile();
            if (!SubtitleFormat.value(QFileInfo(filename).suffix())) {
                if (d->contains(filename)) {
                    continue;
                } else {
                    AudioData *data = new AudioData(url, this);
                    connect(data, &AudioData::created, this, [url, curMusic, this, data] {
                        d->m_musicModel->append(data);
                        if (url == curMusic) {
                            setCurMusic(data);
                        }
                        emit d->m_musicModel->modelChanged();
                    }, Qt::QueuedConnection);
                    data->create();
                }
            }
        }
        d->m_settings->endArray();
        d->m_settings->endGroup();
    }
}

void MusicPlayer::writeSettings()
{
    if (d->m_settings->status() == QSettings::NoError) {
        d->m_settings->beginGroup("MusicPlayer");
        d->m_settings->setValue("SkinName", d->m_skinName);
        if (d->m_curMusic) d->m_settings->setValue("CurMusic", d->m_curMusic->filename());
        d->m_settings->setValue("PlayMode", int(d->m_playMode));
        d->m_settings->beginWriteArray("MusicList");
        for (int i = 0; i < d->m_musicModel->count(); i++) {
           d->m_settings->setArrayIndex(i);
           d->m_settings->setValue("filename", d->m_musicModel->at(i)->filename());
        }
        d->m_settings->endArray();
        d->m_settings->endGroup();
    }
}
