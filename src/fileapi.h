#ifndef API_H
#define API_H

#include <QObject>

class FileApi : public QObject
{
    Q_OBJECT

public:
    FileApi(QObject *parent = nullptr);

    Q_INVOKABLE QString toFileString(const QUrl &url);
    Q_INVOKABLE QString baseName(const QUrl &url);
};

#endif
