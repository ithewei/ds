#ifndef HDSCONTEXT_H
#define HDSCONTEXT_H

#include <QObject>
#include "ds_version.h"
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
    int parse_layout_xml(const char* xml_file);
    int parse_comb_xml(const char* xml);
    int parse_audio_xml(const char* xml);
    int parse_taskinfo_xml(int srvid, const char* xml);

    void start_gui_thread();

    HCombItem* getCombItem(int srvid);
    HCombItem* getCombItem(QString src);

    DsSrvItem* getSrvItem(int srvid){
        if (srvid < 1 || srvid > DIRECTOR_MAX_SERVS)
            return NULL;
        return &m_srvs[srvid-1];
    }

    DsSrvItem* getSrvItem(QString src){
        for (int i = 0 ; i < DIRECTOR_MAX_SERVS; ++i){
            if (m_srvs[i].src_addr.size() != 0 && src.contains(m_srvs[i].src_addr)){
                return &m_srvs[i];
            }
        }
        return NULL;
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
        qInfo("action=%d this->action=%d", action, this->action);
        if (this->action != action){
            this->action = action;
            if (m_audioPlay){
                if (action > 0){
                    m_audioPlay->pausePlay(false);
                }else{
                    m_audioPlay->pausePlay(true);
                }
            }
        }
        emit actionChanged(action);
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
    void setPlayProgress(int srvid, int progress);
    void setPlayaudioSrvid(int id);

signals:
    void actionChanged(int action);
    void videoPushed(int srvid, bool bFirstFrame);
    void audioPushed(int srvid);
    void sigStop(int srvid);
    void quit();
    void combChanged();
    void voiceChanged();
    void sigProgressNty(int srvid, int progress);
    void requestShow(int srvid);

public:
    void onWndSizeChanged(int srvid, QRect rc);

    void onWndVisibleChanged(int srvid, bool bShow){
        DsSrvItem* pItem = getSrvItem(srvid);
        if (pItem){
            pItem->bShow = bShow;
        }
    }

    void onRequestShowSucceed(int srvid, QRect rc){
        DsSrvItem* pItem = getSrvItem(srvid);
        if (pItem){
            pItem->bShow = true;
            onWndSizeChanged(srvid, rc);
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

    QMap<QString, QString> m_mapTTID2Src;

    DsInitInfo m_tInit;
    DsLayoutInfo m_tLayout;
    DsCombInfo m_tComb;
    DsCombInfo m_tCombUndo;
    int m_preselect[MAXNUM_COMB_ITEM];
    DsPictureInfo m_pics;
    DsTextInfo m_texts;

    std::string cur_path;
    std::string img_path;
    std::string ttf_path;
    std::string ds_path;
    std::string layout_file;

    HAudioPlay* m_audioPlay;

    DsSrvItem m_srvs[DIRECTOR_MAX_SERVS];
    QMap<int ,int> m_mapLmic2Srvid;

    int req_srvid;
    int playaudio_srvid;
    int pre_micphone_srvid;
};

extern HDsContext* g_dsCtx;
class HMainWidget;
extern HMainWidget* g_mainWdg;

#define OUTPUT_SRVID    1

inline bool isOutputSrvid(int srvid){
    return srvid == OUTPUT_SRVID;
}

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
