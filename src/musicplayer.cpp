#include "audiodecoder.h"
#include "imageprovider.h"
#include "musicplayer.h"

#include <QDebug>
#include <QAudioOutput>
#include <QTimer>
#include <QUrl>

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject (parent)
{
    m_playbillProvider = new ImageProvider;
    m_playTimer = new QTimer(this);
    connect(m_playTimer, &QTimer::timeout, this, &MusicPlayer::update);

    /*QThread *thread = new QThread;
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);*/
    m_decoder = new AudioDecoder(this);
    connect(m_decoder, &AudioDecoder::error, this, &MusicPlayer::error);
    connect(m_decoder, &AudioDecoder::hasPlaybill, this, [this](const QImage &playbill) {
        m_playbillProvider->setPixmap(playbill);
        emit playbillChanged();
    });
    connect(m_decoder, &AudioDecoder::resolved, this, [this]() {
        if (m_running) {
            emit titleChanged();
            emit authorChanged();
            emit durationChanged();
            m_audioOutput.reset(new QAudioOutput(m_decoder->format()));
            m_audioOutput->setVolume(m_volume);
            m_audioDevice = m_audioOutput->start();
            m_playTimer->start(100);
        }
    });
    /*m_decoder->moveToThread(thread);
    thread->start();*/
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

QString MusicPlayer::author() const
{
    return m_decoder->author();
}

QString MusicPlayer::album() const
{
    return m_decoder->album();
}

void MusicPlayer::play(const QUrl &url)
{
    suspend();
    setMusic(url);
    m_decoder->stop();
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
    while (m_audioBuffer.size() < m_audioOutput->bytesFree()) {
        QByteArray frame = m_decoder->currentFrame();
        qreal currentTime = m_decoder->currentTime();
        qreal duration = m_decoder->duration();
        if (currentTime >= duration) {
            m_progress = 1.0;
            m_running = false;
            m_decoder->stop();
            m_playTimer->stop();
            emit finished();
            emit progressChanged();
            break;
        } else {
            m_progress = currentTime / duration;
            emit progressChanged();
        }

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
