#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QQmlListProperty>
#include <QUrl>

class AudioData  : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal duration READ duration CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString singer READ singer CONSTANT)
    Q_PROPERTY(QString album READ album CONSTANT)
    Q_PROPERTY(QUrl filename READ filename CONSTANT)

public:
    AudioData(const QUrl &filename = QUrl(), QObject *parent = nullptr);

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
    Q_PROPERTY(QQmlListProperty<AudioData> model READ model NOTIFY modelChanged)

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

    QQmlListProperty<AudioData> model();
    void setModel(const QVector<AudioData *> &music);
    int indexof(AudioData *const &music);

    void append(AudioData *music);
    int count() const;
    AudioData* at(int index);
    void clear();

signals:
    void modelChanged();

private:
    static void append(QQmlListProperty<AudioData> *list, AudioData *music);
    static int count(QQmlListProperty<AudioData> *list);
    static AudioData *at(QQmlListProperty<AudioData> *list, int index);
    static void clear(QQmlListProperty<AudioData> *list);

    QVector<AudioData *> m_list;
};

#endif // MUSICMODEL_H
