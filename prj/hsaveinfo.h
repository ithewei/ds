#ifndef HSAVEINFO_H
#define HSAVEINFO_H

#include "ds_def.h"
#include "hglwidget.h"
#include <QSettings>
#include "singleton.h"

struct WndInfo{
    HGLWidget::TYPE type;
    int wndid;
    QRect rc;
    bool visible;
    int srvid;

    WndInfo(){
        type = HGLWidget::UNKOWN;
        wndid = 0;
        visible = false;
        srvid = 0;
    }
};

class HSaveInfo
{
    DECLARE_SINGLETON(HSaveInfo)
public:
    HSaveInfo();
    ~HSaveInfo();

    void read();
    void write();

public:
    QSettings* set;
    WndInfo wndinfo[MAXNUM_WND];
    int wnd_num;
};

#endif // HSAVEINFO_H
