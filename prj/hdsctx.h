#ifndef HDSCONTEXT_H
#define HDSCONTEXT_H

#define DS_EVENT_PICK               0x01
#define DS_EVENT_STOP               0x02

#define DISPLAY_MODE_REALTIME      1  // realtime display
#define DISPLAY_MODE_TIMER         2  // timer by fps

#define NONE_SCALE                 0
#define BIG_VIDEO_SCALE            1

#include <QObject>
#include "ds_def.h"
#include "ds_global.h"
#include "haudioplay.h"

class HDsContext : public QObject
{
    Q_OBJECT

public:
    HDsContext();
    ~HDsContext();

public:
    int parse_init_xml(const char* xml);
    int parse_layout_xml(const char* xml_file);
    int parse_comb_xml(const char* xml);
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

    void fullscreen(int svrid, bool bFull);

signals:
    void actionChanged(int action);
    void videoPushed(int svrid, bool bFirstFrame);
    void audioPushed(int svrid);
    void sigStop(int svrid);
    void quit();
    void combChanged();
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
    DsScreenInfo m_tComb;
    DsScreenInfo m_tCombUndo;
    int m_preselect[MAXNUM_COMB_SCREEN];

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
class HMainWidget;
extern HMainWidget* g_mainWdg;

#endif // HDSCONTEXT_H
