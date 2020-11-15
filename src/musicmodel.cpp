#include "audiodecoder.h"
#include "musicmodel.h"

#include <QtConcurrentRun>
#include <QFileInfo>

extern "C"
{
#include <libavformat/avformat.h>
}

MusicData::MusicData(const QUrl &filename, QObject *parent)
    : QObject (parent)
    , m_filename(filename)
{

}

qreal MusicData::duration() const
{
    return m_duration;
}

QString MusicData::title() const
{
    return m_title;
}

QString MusicData::singer() const
{
    return m_singer;
}

QString MusicData::album() const
{
    return m_album;
}

QUrl MusicData::filename() const
{
    return m_filename;
}

void MusicData::create()
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
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_title < d2->m_title;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_title > d2->m_title;
            });
        }
        break;
    case SortKey::Duration:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_duration < d2->m_duration;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_duration > d2->m_duration;
            });
        }
        break;
    case SortKey::Singer:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_singer < d2->m_singer;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_singer > d2->m_singer;
            });
        }
        break;
    case SortKey::Album:
        if (mode == SortMode::Less) {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_album < d2->m_album;
            });
        } else {
            std::sort(m_list.begin(), m_list.end(), [](MusicData *d1, MusicData *d2)->bool {
                return d1->m_album > d2->m_album;
            });
        }
        break;
    }

    emit modelChanged();
}

QQmlListProperty<MusicData> MusicModel::model()
{
    return QQmlListProperty<MusicData>(this, this,
                                       &MusicModel::append,
                                       &MusicModel::count,
                                       &MusicModel::at,
                                       &MusicModel::clear);
}

void MusicModel::setModel(const QVector<MusicData *> &music)
{
    m_list.clear();
    m_list = music;
    emit modelChanged();
}

int MusicModel::indexof(MusicData *const &music)
{
    return m_list.indexOf(music);
}

void MusicModel::append(MusicData *music)
{
    return m_list.append(music);
}

int MusicModel::count() const
{
    return m_list.count();
}

MusicData* MusicModel::at(int index)
{
    return m_list.at(index);
}

void MusicModel::clear()
{
    for (auto it : m_list) it->deleteLater();
    return m_list.clear();
}

void MusicModel::append(QQmlListProperty<MusicData> *list, MusicData *music)
{
    return reinterpret_cast<MusicModel*>(list->data)->append(music);
}

int MusicModel::count(QQmlListProperty<MusicData> *list)
{
    return reinterpret_cast<MusicModel*>(list->data)->count();
}

MusicData* MusicModel::at(QQmlListProperty<MusicData> *list, int index)
{
    return reinterpret_cast<MusicModel*>(list->data)->at(index);
}

void MusicModel::clear(QQmlListProperty<MusicData> *list)
{
    return reinterpret_cast<MusicModel*>(list->data)->clear();
}
