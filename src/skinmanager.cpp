#include "skinmanager.h"

#include <QFile>

SkinManager* SkinManager::instance()
{
    static SkinManager skin;
    return &skin;
}

SkinManager::~SkinManager()
{

}

/**
 * @brief 两种读取方式
 *   READ(globalColor)
 *   else READ(menuColor)
 * 或者(性能稍差)
 *   globalColorRead();
 *   menuColorRead();
 */
bool SkinManager::loadSkin(const QString &skinName)
{
    QFile file(skinName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return false;
    }

    m_reader.setDevice(&file);
    if (m_reader.readNextStartElement()) {
        qDebug().noquote().nospace() << "<===Load Skin: " << m_reader.name() << "===>";
        while(!m_reader.atEnd()) {
            m_reader.readNext();
            if (m_reader.isStartElement() && m_reader.name() == QLatin1String("color")) {
                globalColorRead(); detailColorRead(); borderColorRead();
                menuColorRead(); menuBackColorRead(); menuHoverColorRead();
                headerColorRead(); headerBorderColorRead(); headerHoverColorRead();
                progressColorRead(); progressBackColorRead(); progressHandleColorRead();
                buttonColorRead(); buttonBackColorRead();
                tooltipColorRead(); tooltipBorderColorRead();
                textColorRead(); musicTextColorRead(); musicTextSelectedColorRead();
            } else if (m_reader.isStartElement() && m_reader.name() == QLatin1String("image")) {
                backgroundImageRead(); progressHandleImageRead();
                buttonParticleImageRead(); progressParticleImageRead();
            }
        }
    }

    file.close();
    return true;
}

SkinManager::SkinManager()
{

}
