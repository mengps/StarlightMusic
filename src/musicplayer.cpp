#include "audiodecoder.h"
#include "musicplayer.h"

#include <QAudioOutput>
#include <QTimer>
#include <QUrl>
#include <QDebug>

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject (parent)
{
    m_playTimer = new QTimer(this);
    connect(m_playTimer, &QTimer::timeout, this, &MusicPlayer::update);
    m_decoder = new AudioDecoder(this);
    connect(m_decoder, &AudioDecoder::error, this, &MusicPlayer::error);
    connect(m_decoder, &AudioDecoder::resolved, this, [this]() {
        if (m_running) {
            emit titleChanged();
            emit durationChanged();
            m_audioOutput.reset(new QAudioOutput(m_decoder->format()));
            m_audioDevice = m_audioOutput->start();
            m_playTimer->start(100);
        }
    });
}

MusicPlayer::~MusicPlayer()
{
    suspend();
    m_decoder->stop();
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
    }
}

qreal MusicPlayer::duration() const
{
    return m_decoder->duration();
}

bool MusicPlayer::running() const
{
    return m_running;
}

QString MusicPlayer::title() const
{
    return m_decoder->title();
}

void MusicPlayer::play(const QUrl &url)
{
    suspend();
    setMusic(url);
    if (m_decoder->isRunning()) {
        m_decoder->stop();
    }
    m_running = true;
    m_progress = 0.0;
    emit progressChanged();
    m_audioBuffer.clear();
    m_decoder->open(url.toLocalFile());
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
    if (m_decoder->currentTime() >= m_decoder->duration()) {
        m_progress = 1.0;
        m_running = false;
        m_playTimer->stop();
        emit finished();
    } else m_progress = m_decoder->currentTime() / m_decoder->duration();
    emit progressChanged();

    while (m_audioBuffer.size() < m_audioOutput->bytesFree()) {
        QByteArray frame = m_decoder->currentFrame();
        if (frame.isEmpty()) break;
        m_audioBuffer += frame;
    }

    int readSize = m_audioOutput->periodSize();
    int chunks = m_audioOutput->bytesFree() / readSize;
    while (chunks--) {
        QByteArray pcm = m_audioBuffer.mid(0, readSize);
        int size = pcm.size();

        if (m_audioBuffer.size() >= size)
            m_audioBuffer.remove(0, size);
        if (size) m_audioDevice->write(pcm);
        if (size != readSize) break;
    }
}
