#ifndef HNETWORK_H
#define HNETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "ds_def.h"
#include "singleton.h"

class HNetwork : public QObject
{
    Q_OBJECT

    DECLARE_SINGLETON(HNetwork)
private:
    HNetwork();

    void networkErrHandler(int errCode);

public:
    void addItem(HAbstractItem* item);
    void removeItem(HAbstractItem* item);
    void modifyItem(HAbstractItem* item);
    void queryItem(HAbstractItem* item);

    void postCombInfo(DsCombInfo& info);

    void addPicture(HPictureItem& item);
    void addText(HTextItem& item);

    void removePicture(HPictureItem& item);
    void removeText(HTextItem& item);

    void modifyPicture(HPictureItem& item);
    void modifyText(HTextItem& item);

    //void queryMicphone();
    void setMicphone(int srvid);


    void setVoice(int srvid, int a);

    void notifyFullscreen(bool bFullscreen);

signals:
    void overlayChanged();

public slots:
    void queryOverlayInfo();
    void onQueryOverlayReply(QNetworkReply* reply);

    void queryVoice();
    void onQueryVoiceReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_nam_post_screeninfo;
    QNetworkAccessManager* m_nam_add_overlay;
    QNetworkAccessManager* m_nam_query_overlay;
    QNetworkAccessManager* m_nam_remove_overlay;
    QNetworkAccessManager* m_nam_modify_overlay;
    QNetworkAccessManager* m_nam_micphone;
    QNetworkAccessManager* m_nam_voice;
    QNetworkAccessManager* m_nam_query_voice;
    QNetworkAccessManager* m_nam_post_notify;
};

#define dsnetwork HNetwork::instance()

#endif // HNETWORK_H
