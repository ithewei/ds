#ifndef DS_DEF_H
#define DS_DEF_H

#include "ds_global.h"
#include <QRect>

#define MAXNUM_LAYOUT   64
#define MAXNUM_COMB_SCREEN     8

#define DIRECTOR_MAX_SERVS		   64
#define DIRECTOR_SRVID_BEGIN       1
#define DIRECTOR_SRVID_END         32
#define DIRECTOR_LMICID_BEGIN      33
#define DIRECTOR_LMICID_END        64

#define DISPLAY_MODE_REALTIME      1  // realtime display
#define DISPLAY_MODE_TIMER         2  // timer by fps

#define DISABLE_SCALE                 0
#define ENABLE_SCALE            1

struct DsInitInfo{
    unsigned int infcolor;
    unsigned int titcolor;
    unsigned int outlinecolor;
    unsigned int focus_outlinecolor;

    unsigned int audiocolor_bg;
    unsigned int audiocolor_fg_low;
    unsigned int audiocolor_fg_high;

    int debug;
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

        audiocolor_bg = 0x0000FFFF;
        audiocolor_fg_low = 0x00FF00FF;
        audiocolor_fg_high = 0xFF0000FF;

        debug = 0;
        autolayout = 0;
        row = 0;
        col = 0;

        audio = 1;
        display_mode = DISPLAY_MODE_TIMER;
        scale_mode = ENABLE_SCALE;
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
struct DsSrvItem{
    bool bUsed;
    bool bPause;
    bool bVoice;

    int src_type;

    std::string title;
    int pic_w;
    int pic_h;
    int framerate;
    bool bShow;
    int wnd_w;
    int wnd_h;
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

    unsigned long tick;
    std::string taskinfo;

    int pop_video_failed_cnt;

    DsSrvItem(){
        init();
    }

    ~DsSrvItem(){
        release();
    }

    void init(){
        bUsed = false;
        bPause = false;
        bVoice = false;
        bShow = false;
        wnd_w = 0;
        wnd_h = 0;
        pic_w = 0;
        pic_h = 0;
        show_w = 0;
        show_h = 0;
        framerate = 0;

        title.clear();
        video_buffer = NULL;
        pSwsCtx = NULL;

        src_type = 0;

        a_channels = 0;
        a_average[0] = 0;
        a_average[1] = 0;
        bUpdateAverage = false;

        a_input = 0;
        v_input = 0;

        ifcb = NULL;

        pop_video_failed_cnt = 0;
    }

    void release(){
        if (video_buffer){
            delete video_buffer;
            video_buffer = NULL;
        }
        tex_yuv.release();
        free_scale();
    }

    void free_scale(){
        if (pSwsCtx){
            sws_freeContext(pSwsCtx);
            pSwsCtx = NULL;
        }
    }

    bool canShow(){
        if (bShow && wnd_w > 0 && wnd_h > 0)
            return true;
        return false;
    }

    bool isAdjustRatio(double* ratio){
        // adjust ratio to decide show_w and show_h
        if (pic_w == 0 || pic_h == 0 || wnd_w == 0 || wnd_h == 0)
            return false;

        double pic_ratio = (double)pic_w / (double)pic_h;
        double wnd_ratio = (double)wnd_w / (double)wnd_h;
        if (qAbs(pic_ratio - wnd_ratio) > 0.5){
            *ratio = pic_ratio / wnd_ratio;
            return true;
        }

        *ratio = 1.0;
        return false;
    }

    void adjustRatio(double ratio){
        show_w = wnd_w;
        show_h = wnd_h;
        if (ratio > 1.0){
            show_h /= ratio;
        }else{
            show_w *= ratio;
        }
    }

    bool isAdjustScale(int* w, int* h){
        if (pic_w > 2*show_w || pic_h > 2*show_h){
            *w = show_w >> 2 << 2;
            *h = show_h;
            return true;
        }

        return false;
    }

    void adjustScale(int w, int h){
        if (w != pic_w || w != pic_h){
            free_scale();
            pSwsCtx = sws_getContext(pic_w,pic_h,AV_PIX_FMT_YUV420P,
                                     w,h,AV_PIX_FMT_YUV420P,
                                     SWS_POINT, NULL, NULL, NULL);
        }
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
