#include "audiodecoder.h"
#include "imageprovider.h"
#include "musicplayer.h"

#include <QDebug>
#include <QAudioOutput>
#include <QFileInfo>
#include <QTimer>
#include <QUrl>

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject (parent)
{
    m_playbillProvider = new ImageProvider;
    m_playTimer = new QTimer(this);
    connect(m_playTimer, &QTimer::timeout, this, &MusicPlayer::update);

    m_lyricsModel = new LyricsModel(this);
    m_lrcDecoder.reset(new LrcDecoder);
    m_decoder = new AudioDecoder(this);
    connect(m_decoder, &AudioDecoder::error, this, &MusicPlayer::error);
    connect(m_decoder, &AudioDecoder::hasPlaybill, this, [this](const QImage &playbill) {
        m_playbillProvider->setPixmap(playbill);
        emit playbillChanged();
    });
    connect(m_decoder, &AudioDecoder::resolved, this, [this]() {
        m_running = true;
        m_title = m_decoder->title();
        m_singer = m_decoder->singer();
        m_album = m_decoder->album();
        m_duration = m_decoder->duration();
        emit titleChanged();
        emit singerChanged();
        emit albumChanged();
        emit durationChanged();

        m_audioOutput.reset(new QAudioOutput(m_decoder->format()));
        m_audioOutput->setVolume(m_volume / qreal(100.0));
        m_audioDevice = m_audioOutput->start();
        m_playTimer->start(100);
    });
}

MusicPlayer::~MusicPlayer()
{
    suspend();
}

ImageProvider *MusicPlayer::imageProvider()
{
    return m_playbillProvider;
}

QUrl MusicPlayer::music() const
{
    return m_music;
}

void MusicPlayer::setMusic(const QUrl &url)
{
    if (url != m_music) {
        m_music = url;
        emit musicChanged();
    }
}

qreal MusicPlayer::progress() const
{
    return m_progress;
}

void MusicPlayer::setProgress(qreal ratio)
{
    if (m_running && qAbs(ratio - m_progress) > 0.000001) {
        m_progress = ratio;
        m_audioBuffer.clear();
        emit progressChanged();
        m_decoder->setProgress(ratio);
        if (m_hasLyrics) {
            int64_t pts = int64_t(ratio * m_duration * 1000);
            int size = m_lyricsModel->size();
            for (int i = 0; i < size; i++) {
                if (m_lyricsModel->at(i)->pts() > pts) {
                    m_lyricIndex = (i > 0) ? (i - 1) : 0;
                    m_nextIndex = (m_lyricIndex + 1) < size ? m_lyricIndex + 1 : size;
                    emit lyricIndexChanged();
                    break;
                }
            }
        }
    }
}

int MusicPlayer::volume() const
{
    return m_volume;
}

void MusicPlayer::setVolume(int vol)
{
    if (vol != m_volume) {
        m_volume = vol;
        if (m_audioOutput) {
            m_audioOutput->setVolume(vol / qreal(100.0));
        }
        emit volumeChanged();
    }
}

qreal MusicPlayer::duration() const
{
    return m_duration;
}

bool MusicPlayer::running() const
{
    return m_running;
}

QString MusicPlayer::title() const
{
    return m_title;
}

QString MusicPlayer::singer() const
{
    return m_singer;
}

QString MusicPlayer::album() const
{
    return m_album;
}

int MusicPlayer::lyricIndex() const
{
    return m_lyricIndex;
}

LyricsModel *MusicPlayer::lyrics() const
{
    return m_lyricsModel;
}

void MusicPlayer::play(const QUrl &url)
{
    suspend();
    setMusic(url);
    m_running = false;
    m_progress = 0.0;
    emit progressChanged();
    m_audioBuffer.clear();
    m_hasLyrics = false;

    QString filename = url.toLocalFile();
    m_decoder->open(filename);

    m_lyricIndex = 0;
    m_nextIndex = 0;
    int suffixLength = QFileInfo(filename).suffix().length();
    QString lrcFile = filename.mid(0, filename.length() - suffixLength - 1) + ".lrc";
    if (QFileInfo::exists(lrcFile)) {
        if (m_lrcDecoder->decode(lrcFile.toLocal8Bit().data())) {
            //创建Model
            QList<LyricData *> model;
            lyricPacket packet = m_lrcDecoder->readPacket();
            while (!packet.isEmpty()) {
                LyricData *data = new LyricData(QString::fromStdString(packet.lyric), packet.pts);
                model.append(data);
                packet = m_lrcDecoder->readPacket();
            }
            m_lyricsModel->setModel(model);
            m_hasLyrics = true;
            if (m_nextIndex + 1 < m_lyricsModel->size()) m_nextIndex++;
            //打印LRC元数据
            m_lrcDecoder->dumpMetadata(stdout);
        }
    }
}

void MusicPlayer::suspend()
{
    if(m_playTimer->isActive())
        m_playTimer->stop();
}

void MusicPlayer::resume()
{
    if (m_running) m_playTimer->start(100);
    else if (!m_music.isEmpty()) play(m_music);
}

void MusicPlayer::update()
{
    while (m_audioBuffer.size() < m_audioOutput->bytesFree()) {
        AudioPacket packet = m_decoder->currentPacket();   
        QByteArray data = packet.data;
        qreal currentTime = packet.time;
        if (currentTime >= m_duration || (data.isEmpty() && currentTime < 0.00000001)) {
            m_progress = 1.0;
            m_running = false;
            m_decoder->stop();
            m_playTimer->stop();
            emit finished();
            emit progressChanged();
            return;
        } else {
            if (m_hasLyrics) {
                int64_t pts = int64_t(currentTime) * 1000;
                if (pts > m_lyricsModel->at(m_lyricIndex)->pts() && pts > m_lyricsModel->at(m_nextIndex)->pts()) {
                    m_lyricIndex = m_nextIndex;
                    if ((m_nextIndex + 1) < m_lyricsModel->size()) m_nextIndex++;
                    emit lyricIndexChanged();
                }
            }
            m_progress = currentTime / m_duration;
            emit progressChanged();
        }

        if (data.isEmpty()) break;
        m_audioBuffer += data;
    }

    int readSize = m_audioOutput->periodSize();
    int chunks = m_audioBuffer.size() / readSize;
    while (chunks--) {
        QByteArray pcm = m_audioBuffer.mid(0, readSize);
        int size = pcm.size();
        m_audioBuffer.remove(0, size);

        if (size) m_audioDevice->write(pcm);
        if (size != readSize) break;
    }
}
