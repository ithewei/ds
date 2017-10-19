#ifndef DS_DEF_H
#define DS_DEF_H

#include <QRect>

#define MAXNUM_LAYOUT   8
#define MAXNUM_COMB_SCREEN     8

#define DIRECTOR_MAX_SERVS		   16
#define DIRECTOR_MAX_ITEMS   		6

struct DsInitInfo{
    int audio;
    int play_audio;
    int info;
    int title;
    unsigned int infcolor;
    unsigned int titcolor;
    unsigned int outlinecolor;
    unsigned int focus_outlinecolor;
    int scale_method;
    int pause_method;

    DsInitInfo(){
        audio = 1;
        play_audio = 0;
        info  = 0;
        title  = 1;
        infcolor = 0x00FF00FF;
        titcolor = 0xFF5A1EFF;
        outlinecolor = 0xFFFFFFFF;
        focus_outlinecolor = 0xFF0000FF;
        scale_method = 0;
        pause_method = 0;
    }
};

struct DsLayoutInfo{
    int width;
    int height;
    QRect items[MAXNUM_LAYOUT];
    int itemCnt;
    int combW;
    int combH;

    DsLayoutInfo(){
        width = 0;
        height = 0;
        itemCnt = 0;

        combW = 0;
        combH = 0;
    }
};

#define DS_EVENT_PICK               0x01
#define DS_EVENT_STOP               0x02

struct DsEvent{
    int type;

    int src_srvid;
    int src_x;
    int src_y;

    int dst_srvid;
    int dst_x;
    int dst_y;
};

#include "habstractitem.h"
struct DsScreenInfo{
    enum COMB_TYPE{
        UNKNOW = 0,
        PIP = 1, //pic in pic 画中画，主画面不能移动和缩放
        TILED = 2, // //平铺，不分主画面和子画面，都能移动和缩放
    };

    int width;
    int height;

    HScreenItem items[MAXNUM_COMB_SCREEN];
    int itemCnt;

    COMB_TYPE comb_type;
    int micphone;

    DsScreenInfo(){
        width = 0;
        height = 0;
        itemCnt = 0;
        comb_type = UNKNOW;
        micphone = 0;
    }
};

#include <QMutex>
#include "qglwidgetimpl.h"
struct DsSvrItem{
    bool bPause;

    int src_type;

    std::string title;
    int width;
    int height;
    Texture tex_yuv;
    bool bUpdateVideo;
    QMutex  mutex;

    int a_channels;
    unsigned short a_average[2];
    bool bUpdateAverage;

    unsigned int a_input;
    unsigned int v_input;
    ifservice_callback * ifcb;

    bool bVoice;

    DsSvrItem(){
        init();
    }

    ~DsSvrItem(){

    }

    void init(){
        width = 0;
        height = 0;
        title.clear();
        tex_yuv.release();
        bUpdateVideo = true;

        src_type = 0;
        bPause = false;
        a_channels = 0;
        a_average[0] = 0;
        a_average[1] = 0;
        bUpdateAverage = false;

        a_input = 0;
        v_input = 0;

        ifcb = NULL;
        bVoice = false;
    }
};

struct intf_s
{
    unsigned int loss;
    unsigned int speed;
    std::string name;
};

struct udptrf_s
{
    unsigned int received;
    unsigned int dispatch;
    unsigned int resend;
    unsigned int loss;
    unsigned int buffer;
    unsigned int overflow;

    intf_s intf[8];
};

inline void init_udptrf_s(udptrf_s * s)
{
    s->received = 0;
    s->dispatch = 0;
    s->resend   = 0;
    s->loss     = 0;
    s->buffer   = 0;
    s->overflow = 0;
    for(int i = 0; i < 8; i++)
    {
        s->intf[i].loss  = 0;
        s->intf[i].speed = 0;
        s->intf[i].name  = "";
    }
}

#endif // DS_DEF_H
