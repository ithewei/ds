#ifndef HDSCONTEXT_H
#define HDSCONTEXT_H

#include <QObject>
#include "ds.h"
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

    HScreenItem* getScreenItem(int srvid);

    DsSrvItem* getSrvItem(int srvid){
        if (srvid < 1 || srvid > DIRECTOR_MAX_SERVS)
            return NULL;
        return &m_tItems[srvid-1];
    }

    int lmicid2srvid(int lmicid){
        QMap<int,int>::const_iterator iter = m_mapLmic2Srvid.find(lmicid);
        if (iter != m_mapLmic2Srvid.end()){
            return iter.value();
        }

        return -1;
    }

    int srvid2lmicid(int srvid){
        QMap<int, int>::const_iterator iter = m_mapLmic2Srvid.begin();
        while (iter != m_mapLmic2Srvid.end()){
            if (iter.value() == srvid)
                return iter.key();
            ++iter;
        }

        return -1;
    }

    int allocLmicid(int lmicid){
        for (int i = DIRECTOR_LMICID_BEGIN; i <= DIRECTOR_LMICID_END; ++i){
            DsSrvItem* item = getSrvItem(i);
            if (!item->bUsed){
                item->bUsed = true;
                item->src_type = SRC_TYPE_LMIC;
                m_mapLmic2Srvid[lmicid] = i;
                return i;
            }
        }
    }

    void freeLmicid(int lmicid){
        m_mapLmic2Srvid.erase(m_mapLmic2Srvid.find(lmicid));
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
        DsSrvItem* item = getSrvItem(srvid);
        if (item){
            item->title = title;
        }
    }
    void stop(int srvid){
        DsSrvItem* item = getSrvItem(srvid);
        if (item){
            item->release();
            item->init();
            emit sigStop(srvid);
        }
    }

    void pause(int srvid, bool bPause);
    void setPlayaudioSrvid(int id);

signals:
    void actionChanged(int action);
    void videoPushed(int srvid, bool bFirstFrame);
    void audioPushed(int srvid);
    void sigStop(int srvid);
    void quit();
    void combChanged();
    void sigProgressNty(int srvid, int progress);
    void requestShow(int srvid);

public:
    void onWndSizeChanged(int srvid, QSize sz);

    void onWndVisibleChanged(int srvid, bool bShow){
        DsSrvItem* pItem = getSrvItem(srvid);
        if (pItem){
            pItem->bShow = bShow;
        }
    }

    void onRequestShowSucceed(int srvid, QSize sz){
        DsSrvItem* pItem = getSrvItem(srvid);
        if (pItem){
            pItem->bShow = true;
            onWndSizeChanged(srvid, sz);
        }
    }

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

    DsSrvItem m_tItems[DIRECTOR_MAX_SERVS];
    QMap<int ,int> m_mapLmic2Srvid;

    int req_srvid;
    int m_playaudio_srvid;
};

extern HDsContext* g_dsCtx;
class HMainWidget;
extern HMainWidget* g_mainWdg;

inline bool isLmic(int srvid){
    if (srvid >= DIRECTOR_LMICID_BEGIN && srvid <= DIRECTOR_LMICID_END)
        return true;
    return false;
}

inline int OUTER_SRVID(int srvid){
    if (isLmic(srvid)){
        return g_dsCtx->srvid2lmicid(srvid);
    }
    return srvid;
}

inline int INNER_SRVID(int srvid){
    if (srvid > 0 && srvid <= DIRECTOR_MAX_SERVS)
        return srvid;
    return g_dsCtx->lmicid2srvid(srvid);
}

#endif // HDSCONTEXT_H
