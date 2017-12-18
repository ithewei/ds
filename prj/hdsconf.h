#ifndef HDSCONF_H
#define HDSCONF_H

#include <QSettings>
#include "singleton.h"

class HDsConf
{
    DECLARE_SINGLETON(HDsConf)
private:
    HDsConf();

public:
    void load(QString filename);
    QString value(QString key);

private:
    QSettings* settings;
};

#endif // HDSCONF_H
