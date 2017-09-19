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

struct DsEvent{
    int type;

    int src_svrid;
    int src_x;
    int src_y;

    int dst_svrid;
    int dst_x;
    int dst_y;
};

struct CommonItem{
    int id;
    QRect rc;
};

struct ScreenItem : public CommonItem{
    int v;
    bool a;
    QString src;
};

struct PictureItem : public CommonItem{
    QString src;
};

struct TextItem : public CommonItem{
    enum TYPE{
        LABEL = 1,
        TIME = 2,
        WATCHER = 3,
        SUBTITLE = 4,
    };

    TYPE type;
    QString text;
    int font_size;
    int font_color;

    TextItem(){
        type = LABEL;
        font_size = 32;
        font_color = 0xFFFFFF;
    }
};

struct DsScreenInfo{
    int width;
    int height;

    ScreenItem items[MAXNUM_COMB_SCREEN];
    int itemCnt;

    DsScreenInfo(){
        width = 0;
        height = 0;
        itemCnt = 0;
    }
};

#include <QMutex>
#include "qglwidgetimpl.h"
struct DsSvrItem{
    bool bPause;

    int src_type;

    std::string title;
    Texture tex_yuv;
    bool bUpdateVideo;
    QMutex  mutex;

    int a_channels;
    unsigned short a_average[2];
    bool bUpdateAverage;

    unsigned int a_input;
    unsigned int v_input;
    ifservice_callback * ifcb;

    DsSvrItem(){
        init();
    }

    ~DsSvrItem(){

    }

    void init(){
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
