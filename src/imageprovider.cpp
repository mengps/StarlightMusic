#include "imageprovider.h"

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    QPixmap pixmap(100, 100);
    pixmap.fill(Qt::white);
    m_pixmap = pixmap;
}

void ImageProvider::setImage(const QImage &image)
{
    if (!image.isNull()) {
        m_pixmap = QPixmap::fromImage(image);
    }
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    m_pixmap.scaled(requestedSize);
    if (size) *size = QSize(requestedSize);

    return m_pixmap;
}
