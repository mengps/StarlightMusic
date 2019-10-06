#include "fileapi.h"

#include <QFileInfo>
#include <QUrl>

FileApi::FileApi(QObject *parent)
    : QObject (parent)
{

}

QString FileApi::toFileString(const QUrl &url)
{
    return url.toLocalFile();
}

QString FileApi::baseName(const QUrl &url)
{
    return QFileInfo(url.toLocalFile()).baseName();
}
