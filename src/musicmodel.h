#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QQmlListProperty>
#include <QUrl>

class MusicData  : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal duration READ duration CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString singer READ singer CONSTANT)
    Q_PROPERTY(QString album READ album CONSTANT)
    Q_PROPERTY(QUrl filename READ filename CONSTANT)

public:
    MusicData(const QUrl &filename = QUrl(), QObject *parent = nullptr);

    void create();

    qreal duration() const;
    QString title() const;
    QString singer() const;
    QString album() const;
    QUrl filename() const;

signals:
    void created();

private:
    qreal m_duration = 0.0;
    QString m_title = "未知";
    QString m_singer = "未知";
    QString m_album = "未知";
    QUrl m_filename = QUrl();

    friend class MusicModel;
    friend class AudioDecoder;
};

class MusicModel : public QObject
{
    Q_OBJECT
    Q_ENUMS(SortKey)
    Q_ENUMS(SortMode)
    Q_PROPERTY(QQmlListProperty<MusicData> model READ model NOTIFY modelChanged)

public: 
    enum class SortKey
    {
        Duration = 1,
        Title,
        Singer,
        Album
    };

    enum class SortMode
    {
        Less = 1,
        Greater
    };

    MusicModel(QObject *parent = nullptr);

    Q_INVOKABLE void sort(SortKey key, SortMode mode);

    QQmlListProperty<MusicData> model();
    void setModel(const QVector<MusicData *> &music);
    int indexof(MusicData *const &music);

    void append(MusicData *music);
    int count() const;
    MusicData* at(int index);
    void clear();

signals:
    void modelChanged();

private:
    static void append(QQmlListProperty<MusicData> *list, MusicData *music);
    static int count(QQmlListProperty<MusicData> *list);
    static MusicData *at(QQmlListProperty<MusicData> *list, int index);
    static void clear(QQmlListProperty<MusicData> *list);

    QVector<MusicData *> m_list;
};

#endif // MUSICMODEL_H
