#ifndef HDSCONTEXT_H
#define HDSCONTEXT_H

#define MAXNUM_LAYOUT   8
#define MAXNUM_COCK     8

#define DIRECTOR_MAX_SERVS		   16
#define DIRECTOR_MAX_ITEMS   		6

#define DIRECTOR_MAX_EVENT   		8
#define DIRECTOR_EVENT_NULL			0
#define DIRECTOR_EVENT_PICK  		1
#define DIRECTOR_EVENT_SELECT 		2
#define DIRECTOR_EVENT_DCLICK		3
#define DIRECTOR_EVENT_QMARK		4
#define DIRECTOR_EVENT_NUMBER		5

#define DIRECTOR_MAX_ICON    		16
#define DIRECTOR_IOCN_NULL		    0
#define DIRECTOR_IOCN_PICK   		1
#define DIRECTOR_IOCN_SELECT 		2
#define DIRECTOR_IOCN_OK  			3
#define DIRECTOR_IOCN_QMARK  		4
#define DIRECTOR_IOCN_NUMBER  		5

#define DIRECTOR_IOCN_BACK          8
#define DIRECTOR_IOCN_PAUSE         9
#define DIRECTOR_IOCN_START        10
#define DIRECTOR_IOCN_SOUND  	   11
#define DIRECTOR_IOCN_NOVIDEO  	   12

#define DS_EVENT_PICK               0x01
#define DS_EVENT_STOP               0x02

#define DISPLAY_MODE_REALTIME      1  // realtime display
#define DISPLAY_MODE_TIMER         2  // timer by fps

#define NONE_SCALE                 0
#define BIG_VIDEO_SCALE            1

#include <QObject>
#include <QRect>
#include "ds_global.h"
#include "haudioplay.h"

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
        info  = 1;
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
    int cockW;
    int cockH;

    DsLayoutInfo(){
        width = 0;
        height = 0;
        itemCnt = 0;

        cockW = 0;
        cockH = 0;
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

struct HRect{
    int x;
    int y;
    int w;
    int h;

    HRect(){
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }
};

struct DsCockItem{
    int x;
    int y;
    int w;
    int h;

    int  iSvrid;
    bool bAudio;

    DsCockItem(){
        x = 0;
        y = 0;
        w = 0;
        h = 0;
        iSvrid = 0;
        bAudio = false;
    }
};

struct DsCockInfo{
    int width;
    int height;

    DsCockItem items[MAXNUM_COCK];
    int itemCnt;

    DsCockInfo(){
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

static void init_udptrf_s(udptrf_s * s)
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

class HDsContext : public QObject
{
    Q_OBJECT

public:
    HDsContext();
    ~HDsContext();

public:
    int parse_init_xml(const char* xml);
    int parse_layout_xml(const char* xml_file);
    int parse_cock_xml(const char* xml);
    int parse_taskinfo_xml(const char* xml);

    void initImg(std::string& path);
    void initFont(std::string& path, int h);

    void start_gui_thread();
    void handle_event(DsEvent& event);

    DsSvrItem* getItem(int svrid){
        if (svrid < 1 || svrid > DIRECTOR_MAX_SERVS)
            return NULL;
        return &m_tItems[svrid-1];
    }

    int push_video(int svrid, const av_picture* pic);
    int push_audio(int svrid, const av_pcmbuff* pcm);

    void setAction(int action) {
        qDebug("");
        this->action = action;
        emit actionChanged(action);
    }

    void setTitle(int svrid, const char* title){
        DsSvrItem* item = getItem(svrid);
        if (item){
            item->title = title;
            emit titleChanged(svrid);
        }
    }
    void stop(int svrid){
        qDebug("");
        DsSvrItem* item = getItem(svrid);
        if (item){
            item->init();
        }
        emit sigStop(svrid);
    }

    void setFilter(int svrid){
        filter = svrid;
    }
    void cancelFilter(){
        filter = 0;
    }
    void fullscreen(int svrid, bool bFull);

signals:
    void actionChanged(int action);
    void titleChanged(int svrid);
    void videoPushed(int svrid, bool bFirstFrame);
    void audioPushed(int svrid);
    void sourceChanged(int svrid, bool bSucceed);
    void sigStop(int svrid);
    void quit();
    void cockChanged();
    void sigProgressNty(int svrid, int progress);

public slots:

public:
#ifdef WIN32
static void thread_gui(void* param);
#else
static void* thread_gui(void* param);
#endif


public:
    int ref;
    int init;
    QMutex m_mutex;
    int action; // window show or hide
    int display_mode;
    int frames;
    int scale_mode;
    int filter;

    DsInitInfo m_tInit;
    DsLayoutInfo m_tLayout;
    DsCockInfo m_tCock;
    DsCockInfo m_tCockUndo;
    int m_preselect[MAXNUM_COCK];

    std::string img_path;
    std::string ttf_path;

    FTGLPixmapFont* m_pFont;
    HAudioPlay* m_audioPlay;
    transaction* m_trans;

    DsSvrItem m_tItems[DIRECTOR_MAX_SERVS];

    std::string m_strTaskInfo;
    uint m_curTick;
    uint m_lastTick;
};

extern HDsContext* g_dsCtx;

#endif // HDSCONTEXT_H
