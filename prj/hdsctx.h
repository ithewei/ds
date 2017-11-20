#ifndef HDSCONTEXT_H
#define HDSCONTEXT_H

#include <QObject>
#include "ds_def.h"
#include "ds_global.h"
#include "haudioplay.h"

void myLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg);

class HDsContext : public QObject
{
    Q_OBJECT

public:
    HDsContext();
    ~HDsContext();

public:
    //int parse_init_xml(const char* xml); //DEPRECATED
    int parse_layout_xml(const char* xml_file);
    int parse_comb_xml(const char* xml);
    int parse_audio_xml(const char* xml);
    int parse_taskinfo_xml(const char* xml);

    void initImg(std::string& path);
    void initFont(std::string& path, int h);

    void start_gui_thread();

    HScreenItem* getHScreenItem(int srvid);

    DsSvrItem* getItem(int srvid){
        if (srvid < 1 || srvid > DIRECTOR_MAX_SERVS)
            return NULL;
        return &m_tItems[srvid-1];
    }

    int push_video(int srvid, const av_picture* pic);
    int pop_video(int srvid);
    int push_audio(int srvid, const av_pcmbuff* pcm);

    void setAction(int action) {
        if (this->action != action){
            this->action = action;
            emit actionChanged(action);
            if (action > 0){
                m_audioPlay->pausePlay(false);
            }else{
                m_audioPlay->pausePlay(true);
            }
        }
    }

    void setTitle(int srvid, const char* title){
        DsSvrItem* item = getItem(srvid);
        if (item){
            item->title = title;
        }
    }
    void stop(int srvid){
        DsSvrItem* item = getItem(srvid);
        if (item){
            item->init();
        }
        emit sigStop(srvid);
    }

    void pause(int srvid, bool bPause);
    void resizeForScale(int srvid, int w, int h);

    void setPlayaudioSrvid(int id);

signals:
    void actionChanged(int action);
    void videoPushed(int srvid, bool bFirstFrame);
    void audioPushed(int srvid);
    void sigStop(int srvid);
    void quit();
    void combChanged();
    void sigProgressNty(int srvid, int progress);

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

    DsInitInfo m_tInit;
    DsLayoutInfo m_tLayout;
    DsScreenInfo m_tComb;
    DsScreenInfo m_tCombUndo;
    int m_preselect[MAXNUM_COMB_SCREEN];

    std::string img_path;
    std::string ttf_path;

    FTGLPixmapFont* m_pFont;
    HAudioPlay* m_audioPlay;

    DsSvrItem m_tItems[DIRECTOR_MAX_SERVS];

    std::string m_strTaskInfo;
    uint m_curTick;
    uint m_lastTick;

    int m_playaudio_srvid;
};

extern HDsContext* g_dsCtx;
class HMainWidget;
extern HMainWidget* g_mainWdg;

#endif // HDSCONTEXT_H
