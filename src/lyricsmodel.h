#ifndef LYRICSMODEL_H
#define LYRICSMODEL_H

#include <QQmlListProperty>

class LyricData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lyric READ lyric NOTIFY lyricChanged)

public:
    LyricData(QObject *parent = nullptr);
    LyricData(const QString &lyric, int64_t pts, QObject *parent = nullptr);

    QString lyric() const;
    int64_t pts() const;

signals:
    void lyricChanged();

private:
    QString m_lyric = QString();
    int64_t m_pts = 0;
};

class LyricsModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<LyricData> model READ model NOTIFY modelChanged)

public:
    LyricsModel(QObject *parent = nullptr);

    QQmlListProperty<LyricData> model() const;
    void setModel(const QList<LyricData *> &lyrics);

    int size() const;
    LyricData* at(int index);

signals:
    void modelChanged();

private:
    QQmlListProperty<LyricData> *m_proxy;
    QList<LyricData *> m_list;
};

#endif
