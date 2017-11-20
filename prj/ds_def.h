#ifndef DS_DEF_H
#define DS_DEF_H

#include "ds_global.h"
#include <QRect>

#define MAXNUM_LAYOUT   64
#define MAXNUM_COMB_SCREEN     8

#define DIRECTOR_MAX_SERVS		   65 // srvid = 1 reserve

#define DISPLAY_MODE_REALTIME      1  // realtime display
#define DISPLAY_MODE_TIMER         2  // timer by fps

#define NONE_SCALE                 0
#define BIG_VIDEO_SCALE            1

struct DsInitInfo{
    unsigned int infcolor;
    unsigned int titcolor;
    unsigned int outlinecolor;
    unsigned int focus_outlinecolor;

    int autolayout;
    int row;
    int col;

    int audio;
    int display_mode;
    int scale_mode;
    int fps;

    int drawinfo;
    int drawtitle;
    int drawfps;
    int drawnum;
    int drawaudio;

    DsInitInfo(){
        infcolor = 0x00FF00FF;
        titcolor = 0xFF5A1EFF;
        outlinecolor = 0xFFFFFFFF;
        focus_outlinecolor = 0xFF0000FF;

        autolayout = 0;
        row = 0;
        col = 0;

        audio = 1;
        display_mode = DISPLAY_MODE_TIMER;
        scale_mode = BIG_VIDEO_SCALE;
        fps = 25;

        drawinfo = 0;
        drawtitle = 1;
        drawfps = 0;
        drawnum = 1;
        drawaudio = 1;
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
#include "hringbuffer.h"
#include "hffmpeg.h"
struct DsSvrItem{
    bool bPause;
    bool bVoice;

    int src_type;

    std::string title;
    int pic_w;
    int pic_h;
    bool bShow;
    int show_w;
    int show_h;
    HRingBuffer* video_buffer;
    QMutex  mutex;
    Texture tex_yuv;
    SwsContext* pSwsCtx; // for scale

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
        free();
    }

    void free(){
        if (video_buffer){
            delete video_buffer;
            video_buffer = NULL;
        }

        tex_yuv.release();

        if (pSwsCtx){
            sws_freeContext(pSwsCtx);
            pSwsCtx = NULL;
        }
    }

    void init(){
        free();

        bPause = false;
        bVoice = false;
        bShow = true;
        show_w = 0;
        show_h = 0;
        pic_w = 0;
        pic_h = 0;

        title.clear();
        video_buffer = NULL;
        tex_yuv.release();
        pSwsCtx = NULL;

        src_type = 0;

        a_channels = 0;
        a_average[0] = 0;
        a_average[1] = 0;
        bUpdateAverage = false;

        a_input = 0;
        v_input = 0;

        ifcb = NULL;
    }

    bool canShow(){
        if (!bPause && bShow && show_w > 0 && show_h > 0)
            return true;
        return false;
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
