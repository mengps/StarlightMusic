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
    m_proxy = new QQmlListProperty<LyricData>(this, m_list);
}

QQmlListProperty<LyricData> LyricsModel::model() const
{
    return *m_proxy;
}

void LyricsModel::setModel(const QList<LyricData *> &lyrics)
{
    qDeleteAll(m_list);
    m_list.clear();
    m_list = lyrics;
    emit modelChanged();
}

int LyricsModel::size() const
{
    return m_list.size();
}

LyricData* LyricsModel::at(int index)
{
    return m_list.at(index);
}
