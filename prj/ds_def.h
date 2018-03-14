#ifndef DS_DEF_H
#define DS_DEF_H

#include "ds_global.h"
#include <QRect>
#include <QString>

#define MAXNUM_WND   64
#define MAXNUM_COMB_ITEM     8
#define MAXNUM_PICTURE_ITEM    64
#define MAXNUM_TEXT_ITEM       64

#define DIRECTOR_MAX_SERVS		   64
#define OUTPUT_SRVID               1
#define DIRECTOR_SRVID_BEGIN       1
#define DIRECTOR_SRVID_END         32
#define DIRECTOR_LMICID_BEGIN      33
#define DIRECTOR_LMICID_END        64

#define DISPLAY_MODE_REALTIME      1  // realtime display
#define DISPLAY_MODE_TIMER         2  // timer by fps

#define DISABLE_SCALE                 0
#define ENABLE_SCALE            1

#define DEFAULT_AUDIO_BUFNUM    15
#define DEFAULT_VIDEO_BUFNUM    10
#define DEFAULT_LMIC_VIDEO_BUFNUM   3

#define AUDIO_EXCEPTION_CNT 5
#define AUDIO_BUFFER_MAXNUM 50

struct DsInitInfo{
    int debug;
    int save_span;
    int audio;
    int display_mode;
    int scale_mode;
    int fps;
    int audio_bufnum;
    int video_bufnum;

    int autolayout;
    int maxnum_layout;
    int row;
    int col;
    int merge[2];
    int output;

    unsigned int infcolor;
    unsigned int titcolor;
    unsigned int outlinecolor;
    unsigned int focus_outlinecolor;

    int audiostyle;
    unsigned int audiocolor_bg;
    unsigned int audiocolor_fg_low;
    unsigned int audiocolor_fg_high;
    unsigned int audiocolor_fg_top;


    int mouse;
    int fontsize;

    int drawinfo;
    int drawtitle;
    int drawfps;
    int drawnum;
    int drawaudio;
    int drawoutline;
    int drawDebugInfo;

    int spacing;
    int titlebar_height;
    int toolbar_height;
    int output_titlebar_height;
    int output_toolbar_height;

    QString title_format;
    QString taskinfo_format;

    int ptz_num[6];
    int ptz_den[6];

    int maxnum_overlay;
    int expre_policy;

    DsInitInfo(){
        infcolor = 0x00FF00FF;
        titcolor = 0xFF5A1EFF;
        outlinecolor = 0xFFFFFFFF;
        focus_outlinecolor = 0xFF0000FF;
        audiostyle = 1;
        audiocolor_bg = 0x00FFFF80;
        audiocolor_fg_low = 0xFFFF0080;
        audiocolor_fg_high = 0xFF0000FF;
        audiocolor_fg_top = 0xFF0000FF;

        debug = 4;
        drawDebugInfo = 0;
        save_span = 0;
        autolayout = 0;
        maxnum_layout = 0;
        row = 0;
        col = 0;
        merge[0] = 0;
        merge[1] = 0;
        output = 0;

        audio = 1;
        display_mode = DISPLAY_MODE_TIMER;
        scale_mode = ENABLE_SCALE;
        fps = 25;
        video_bufnum = DEFAULT_VIDEO_BUFNUM;
        audio_bufnum = DEFAULT_AUDIO_BUFNUM;

        drawinfo = 0;
        drawtitle = 1;
        drawfps = 0;
        drawnum = 1;
        drawaudio = 1;
        drawoutline = 1;

        fontsize = 24;
        spacing = 20;
        titlebar_height = 48;
        toolbar_height = 64;
        output_titlebar_height = 64;
        output_toolbar_height = 64;

        title_format = "%title";
        taskinfo_format = "%rate";

        maxnum_overlay = 3;
        expre_policy = 0;

        for (int i = 0; i < 6; ++i){
            ptz_num[i] = 1;
            ptz_den[i] = 10;
        }
    }
};

struct DsLayoutInfo{
    int width;
    int height;
    QRect items[MAXNUM_WND];
    int itemCnt;

    DsLayoutInfo(){
        width = 0;
        height = 0;
        itemCnt = 0;
    }
};

#include "habstractitem.h"
struct DsCombInfo{
    enum COMB_TYPE{
        UNKNOW = 0,
        PIP = 1, //pic in pic 画中画，主画面不能移动和缩放
        TILED = 2, // //平铺，不分主画面和子画面，都能移动和缩放
    };

    int width;
    int height;

    HCombItem items[MAXNUM_COMB_ITEM];
    int itemCnt;

    COMB_TYPE comb_type;
    int micphone;
    int pre_micphone;

    DsCombInfo(){
        width = 0;
        height = 0;
        itemCnt = 0;
        comb_type = UNKNOW;
        micphone = 0;
        pre_micphone = 0;
    }
};

struct DsPictureInfo{
    HPictureItem items[MAXNUM_PICTURE_ITEM];
    int itemCnt;

    DsPictureInfo(){
        itemCnt = 0;
    }
};

struct DsTextInfo{
    HTextItem items[MAXNUM_TEXT_ITEM];
    int itemCnt;

    DsTextInfo(){
        itemCnt = 0;
    }
};

struct task_PTZ_ctrl_s
{
    int x[2];
    int y[2];
    int z[2];

    int opt;
    void * ptr;

    task_PTZ_ctrl_s(){
        x[0] = 0;
        x[1] = 10;
        y[0] = 0;
        y[1] = 10;
        z[0] = 0;
        z[1] = 10;

        opt  = 0;
        ptr  = NULL;
    }
};

#include <QMutex>
#include "qglwidgetimpl.h"
#include "hringbuffer.h"
#include "hffmpeg.h"
#include "haudioplay.h"
struct DsSrvItem{
    bool bUsed;
    bool bPause;
    bool bVoice;

    int src_type;
    QString src_addr;
    std::string title;
    bool ptz_enabled;

    int pic_w;
    int pic_h;
    int framerate;
    bool bShow;
    int wnd_w;
    int wnd_h;
    int show_w;
    int show_h;
    HRingBuffer* video_buffer;
    QMutex  video_mutex;
    bool bNeedReallocTexture;
    Texture tex_yuv;
    SwsContext* pSwsCtx; // for scale

    HAudioPlay* audio_player;
    HRingBuffer* audio_buffer;
    QMutex audio_mutex;
    int a_channels;
    int pcmlen;
    int samplerate;
    int audio_empty;
    int audio_full;
    unsigned short a_average[2];
    bool bUpdateAverage;

    unsigned int a_input;
    unsigned int v_input;
    ifservice_callback * ifcb;
    bool spacer;
    bool spacer_activate;

    unsigned long tick;
    QString taskinfo;

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
        bNeedReallocTexture = false;

        title.clear();
        video_buffer = NULL;
        pSwsCtx = NULL;

        src_type = 0;
        src_addr.clear();
        ptz_enabled = false;

        audio_empty = 0;
        audio_full = 0;
        a_channels = 0;
        pcmlen = 0;
        samplerate = 0;
        a_average[0] = 0;
        a_average[1] = 0;
        bUpdateAverage = false;

        audio_player = NULL;
        audio_buffer = NULL;

        a_input = 0;
        v_input = 0;

        ifcb = NULL;
        spacer = false;
        spacer_activate = false;

        tick = 0;
        pop_video_failed_cnt = 0;
    }

    void release(){
        video_mutex.lock();
        if (video_buffer){
            delete video_buffer;
            video_buffer = NULL;
        }
        video_mutex.unlock();

        tex_yuv.release();
        free_scale();

        audio_mutex.lock();
        if (audio_buffer){
            delete audio_buffer;
            audio_buffer = NULL;
        }
        audio_mutex.unlock();

        if (audio_player){
            delete audio_player;
            audio_player = NULL;
        }
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
            *w = show_w >> 3 << 3;
            *h = show_h >> 1 << 1;
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
