#include "hsaveinfo.h"

IMPL_SINGLETON(HSaveInfo)

HSaveInfo::HSaveInfo()
{
    QString file = g_dsCtx->cur_path.c_str();
    file += "dssave.conf";
    set = new QSettings(file, QSettings::IniFormat);

    wnd_num = 0;
}

HSaveInfo::~HSaveInfo(){
    if (set){
        delete set;
        set = NULL;
    }
}

void HSaveInfo::read(){
    set->beginReadArray("WNDS");
    wnd_num = 0;
    for (int i = 0; i < MAXNUM_WND; ++i){
        set->setArrayIndex(i);
        int wndid = set->value("wndid", 0).toInt();
        if (wndid == 0)
            break;

        wndinfo[i].type = (HGLWidget::TYPE)set->value("type").toInt();
        wndinfo[i].wndid = set->value("wndid", 0).toInt();
        wndinfo[i].srvid = set->value("srvid").toInt();
        wndinfo[i].visible = set->value("visible").toBool();
        int x = set->value("x").toInt();
        int y = set->value("y").toInt();
        int w = set->value("w").toInt();
        int h = set->value("h").toInt();
        wndinfo[i].rc.setRect(x, y, w, h);

        wnd_num++;
    }
    set->endArray();
}

void HSaveInfo::write(){
    set->beginWriteArray("WNDS");
    for (int i = 0; i < wnd_num; ++i){
        set->setArrayIndex(i);
        if (wndinfo[i].wndid != 0){
            set->setValue("type", (int)wndinfo[i].type);
            set->setValue("wndid", wndinfo[i].wndid);
            set->setValue("srvid", wndinfo[i].srvid);
            set->setValue("visible", wndinfo[i].visible);
            set->setValue("x", wndinfo[i].rc.x());
            set->setValue("y", wndinfo[i].rc.y());
            set->setValue("w", wndinfo[i].rc.width());
            set->setValue("h", wndinfo[i].rc.height());
        }
    }
    set->endArray();
}
