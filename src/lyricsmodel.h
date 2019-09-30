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

    QQmlListProperty<LyricData> model();
    void setModel(const QVector<LyricData *> &lyrics);

    void append(LyricData *lyric);
    int count() const;
    LyricData* at(int index);
    void clear();

signals:
    void modelChanged();

private:
    static void append(QQmlListProperty<LyricData> *list, LyricData *lyric);
    static int count(QQmlListProperty<LyricData> *list);
    static LyricData *at(QQmlListProperty<LyricData> *list, int index);
    static void clear(QQmlListProperty<LyricData> *list);

    QVector<LyricData *> m_list;
};

#endif
