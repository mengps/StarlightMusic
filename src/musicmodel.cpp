#include "audiodecoder.h"
#include "musicmodel.h"

#include <QtConcurrentRun>
#include <QFileInfo>

extern "C"
{
#include <libavformat/avformat.h>
}

AudioData::AudioData(const QUrl &filename, QObject *parent)
    : QObject (parent)
    , m_filename(filename)
{

}

qreal AudioData::duration() const
{
    return m_duration;
}

QString AudioData::title() const
{
    return m_title;
}

QString AudioData::singer() const
{
    return m_singer;
}

QString AudioData::album() const
{
    return m_album;
}

QUrl AudioData::filename() const
{
    return m_filename;
}

void AudioData::create()
{
    QtConcurrent::run([this]{
        AudioDecoder::getAudioInfo(this);
        emit created();
    });
}

MusicModel::MusicModel(QObject *parent)
    : QObject (parent)
{

}

void MusicModel::sort(SortKey key, SortMode mode)
{
    switch (key) {
    case SortKey::Title:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_title < d2->m_title;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_title > d2->m_title;
            });
        }
        break;
    case SortKey::Duration:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_duration < d2->m_duration;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_duration > d2->m_duration;
            });
        }
        break;
    case SortKey::Singer:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_singer < d2->m_singer;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_singer > d2->m_singer;
            });
        }
        break;
    case SortKey::Album:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_album < d2->m_album;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](AudioData *d1, AudioData *d2)->bool {
                return d1->m_album > d2->m_album;
            });
        }
        break;
    }

    emit modelChanged();
}

QQmlListProperty<AudioData> MusicModel::model()
{
    return QQmlListProperty<AudioData>(this, this,
                                       &MusicModel::append,
                                       &MusicModel::count,
                                       &MusicModel::at,
                                       &MusicModel::clear);
}

void MusicModel::setModel(const QVector<AudioData *> &music)
{
    m_list.clear();
    m_list = music;
    emit modelChanged();
}

int MusicModel::indexof(AudioData *const &music)
{
    return m_list.indexOf(music);
}

void MusicModel::append(AudioData *music)
{
    return m_list.append(music);
}

int MusicModel::count() const
{
    return m_list.count();
}

AudioData* MusicModel::at(int index)
{
    return m_list.at(index);
}

void MusicModel::clear()
{
    for (auto it : m_list) it->deleteLater();
    return m_list.clear();
}

void MusicModel::append(QQmlListProperty<AudioData> *list, AudioData *music)
{
    return reinterpret_cast<MusicModel*>(list->data)->append(music);
}

int MusicModel::count(QQmlListProperty<AudioData> *list)
{
    return reinterpret_cast<MusicModel*>(list->data)->count();
}

AudioData* MusicModel::at(QQmlListProperty<AudioData> *list, int index)
{
    return reinterpret_cast<MusicModel*>(list->data)->at(index);
}

void MusicModel::clear(QQmlListProperty<AudioData> *list)
{
    return reinterpret_cast<MusicModel*>(list->data)->clear();
}
