#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QObject>

class SkinManager : public QObject
{
    Q_OBJECT

public:
    static SkinManager* instance();
    ~SkinManager();

private:
    SkinManager();
};

#endif //SKINMANAGER_H
