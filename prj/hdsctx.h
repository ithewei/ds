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

#include <QObject>
#include <QRect>
#include "ds_global.h"
#include "hmainwidget.h"
#include "haudioplay.h"

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

struct DsCockInfo{
    int x;
    int y;
    int w;
    int h;

    bool bVideo;
    bool bAudio;

    DsCockInfo(){
        x = 0;
        y = 0;
        w = 0;
        h = 0;
        bVideo = false;
        bAudio = false;
    }
};

#include <QMutex>
struct DsItemInfo{
    bool bPause;

    int src_type;

    std::string title;
    Texture tex_yuv;
    QMutex  mutex;

    unsigned short a_average[2];
    bool bUpdateAverage;

    unsigned int a_input;
    unsigned int v_input;
    ifservice_callback * ifcb;

    DsItemInfo(){
        init();
    }

    ~DsItemInfo(){

    }

    void init(){
        title.clear();
        tex_yuv.release();

        src_type = 0;
        bPause = false;
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

    DsItemInfo* getItem(int svrid){
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
        DsItemInfo* item = getItem(svrid);
        if (item){
            item->title = title;
            emit titleChanged(svrid);
        }
    }
    void stop(int svrid){
        qDebug("");
        DsItemInfo* item = getItem(svrid);
        if (item){
            item->init();
        }
        emit sigStop(svrid);
    }

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

    transaction* m_trans;

    int width;
    int height;
    int audio;
    int play_audio;
    int info;
    int title;
    unsigned int infcolor;
    unsigned int titcolor;
    unsigned int outlinecolor;
    int scale_method;
    int pause_method;

    std::string img_path;
    std::string ttf_path;
    FTGLPixmapFont* m_pFont;

    int frames;
    std::string m_strUrl;
    QRect m_rcItems[MAXNUM_LAYOUT];
    int m_cntItem;
    int m_iCockW;
    int m_iCockH;

    DsCockInfo m_tOriginCocks[MAXNUM_COCK];
    int m_cntCock;
    int m_iOriginCockW;
    int m_iOriginCockH;

//    Texture tex_yuv[DIRECTOR_MAX_SERVS];
//    std::string m_title[DIRECTOR_MAX_SERVS];

    HAudioPlay* m_audioPlay;

    DsItemInfo m_tItems[DIRECTOR_MAX_SERVS];

    std::string m_strTaskInfo;
    uint m_curTick;
    uint m_lastTick;
    bool m_bUpdateTaskInfo;
};

extern HDsContext* g_dsCtx;

#endif // HDSCONTEXT_H
