#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QDebug>
#include <QObject>
#include <QUrl>
#include <QXmlStreamReader>

#define READ(attr) \
    if (m_reader.attributes().value(QLatin1String("title")) == QLatin1String(#attr)) { \
        m_##attr = m_reader.readElementText(); \
        emit attr##Changed(); \
        qDebug().noquote().nospace() << #attr << ": " << m_##attr; \
    }

#define COLOR(color) \
    Q_PROPERTY(QString color READ color NOTIFY color##Changed) \
    Q_SIGNALS: void color##Changed(); \
    public: QString color() const { return m_##color; } \
    private: void color##Read() { READ(color); } \
        QString m_##color;

#define IMAGE(image) \
    Q_PROPERTY(QUrl image READ image NOTIFY image##Changed) \
    Q_SIGNALS: void image##Changed(); \
    public: QUrl image() const { return m_##image; } \
    private: void image##Read() { READ(image); } \
        QUrl m_##image;

class SkinManager : public QObject
{
    Q_OBJECT

    //Panel
    COLOR(globalColor) COLOR(detailColor) COLOR(borderColor)
    //Menu
    COLOR(menuColor) COLOR(menuHoverColor) COLOR(menuBackColor)
    //Header
    COLOR(headerColor) COLOR(headerHoverColor) COLOR(headerBorderColor)
    //Progress
    COLOR(progressColor) COLOR(progressBackColor) COLOR(progressHandleColor)
    //Button
    COLOR(buttonColor) COLOR(buttonBackColor)
    //Tooltip
    COLOR(tooltipColor) COLOR(tooltipBorderColor)
    //Text
    COLOR(textColor) COLOR(musicTextColor) COLOR(musicTextSelectedColor)
    //Image
    IMAGE(backgroundImage) IMAGE(progressHandleImage)
    //ParticleImage
    IMAGE(buttonParticleImage) IMAGE(progressParticleImage)

public:
    static SkinManager* instance();
    ~SkinManager();

    Q_INVOKABLE void loadSkin(const QString &skinName);

private:
    SkinManager();

    QXmlStreamReader m_reader;
};

#endif //SKINMANAGER_H
