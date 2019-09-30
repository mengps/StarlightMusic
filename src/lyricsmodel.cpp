#include "lyricsmodel.h"

LyricData::LyricData(QObject *parent)
    : QObject (parent)
{

}

LyricData::LyricData(const QString &lyric, int64_t pts, QObject *parent)
    : QObject (parent)
    , m_lyric(lyric)
    , m_pts(pts)
{

}

QString LyricData::lyric() const
{
    return m_lyric;
}

int64_t LyricData::pts() const
{
    return m_pts;
}

LyricsModel::LyricsModel(QObject *parent)
    : QObject (parent)
{

}

QQmlListProperty<LyricData> LyricsModel::model()
{
    return QQmlListProperty<LyricData>(this, this,
                                       &LyricsModel::append,
                                       &LyricsModel::count,
                                       &LyricsModel::at,
                                       &LyricsModel::clear);
}

void LyricsModel::setModel(const QVector<LyricData *> &lyrics)
{
    for (auto it : m_list) it->deleteLater();
    m_list.clear();
    m_list = lyrics;
    emit modelChanged();
}

void LyricsModel::append(LyricData *lyric)
{
    m_list.append(lyric);
}

int LyricsModel::count() const
{
    return m_list.count();
}

LyricData* LyricsModel::at(int index)
{
    return m_list.at(index);
}

void LyricsModel::clear()
{
    m_list.clear();
}

void LyricsModel::append(QQmlListProperty<LyricData> *list, LyricData *lyric)
{
    return reinterpret_cast<LyricsModel*>(list->data)->append(lyric);
}

int LyricsModel::count(QQmlListProperty<LyricData> *list)
{
    return reinterpret_cast<LyricsModel*>(list->data)->count();
}

LyricData* LyricsModel::at(QQmlListProperty<LyricData> *list, int index)
{
    return reinterpret_cast<LyricsModel*>(list->data)->at(index);
}

void LyricsModel::clear(QQmlListProperty<LyricData> *list)
{
    return reinterpret_cast<LyricsModel*>(list->data)->clear();
}
