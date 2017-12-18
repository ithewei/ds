#include "hdsconf.h"

IMPL_SINGLETON(HDsConf)

HDsConf::HDsConf()
{

}

void HDsConf::load(QString filename){
    settings = new QSettings(filename, QSettings::IniFormat);
}

QString HDsConf::value(QString key){
    return settings->value(key).toString();
}
