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

struct DSItemInfo{
    int x;
    int y;
    int w;
    int h;

    bool bVideo;
    bool bAudio;

    DSItemInfo(){
        x = 0;
        y = 0;
        w = 0;
        h = 0;
        bVideo = false;
        bAudio = false;
    }
};

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

    void initImg(std::string& path);
    void initFont(std::string& path, int h);

    void start_gui_thread();
    int push_video(int svrid, const av_picture* pic);
    int push_audio(int svrid, const av_pcmbuff* pcm);
    void handle_event(DsEvent& event);

    void setAction(int action) {
        qDebug("");
        this->action = action;
        emit actionChanged(action);
    }
    void setInfo(std::string info){
        qDebug("");
    }
    void setTitle(int svrid, std::string title){
        qDebug("");
        emit titleChanged(svrid, title);
    }
    void stop(int svrid){
        tex_yuv[svrid].release();
        emit sigStop(svrid);
    }

signals:
    void actionChanged(int action);
    void titleChanged(int svrid, std::string title);
    void videoPushed(int svrid, bool bFirstFrame);
    void audioPushed(int svrid);
    void sourceChanged(int svrid, bool bSucceed);
    void sigStop(int svrid);
    void quit();

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
    int action; // window show or hide

    unsigned int a_input[DIRECTOR_MAX_SERVS];
    unsigned int v_input[DIRECTOR_MAX_SERVS];

    ifservice_callback * ifcb[DIRECTOR_MAX_SERVS]; // add @ 2017/05/22
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

    std::string m_strUrl;
    QRect m_rcItems[MAXNUM_LAYOUT];
    int m_cntItem;
    int m_iCockW;
    int m_iCockH;

    DSItemInfo m_tOriginCocks[MAXNUM_COCK];
    int m_cntCock;
    int m_iOriginCockW;
    int m_iOriginCockH;

    Texture tex_yuv[DIRECTOR_MAX_SERVS];

    HAudioPlay* m_audioPlay;
};

extern HDsContext* g_dsCtx;

#endif // HDSCONTEXT_H
