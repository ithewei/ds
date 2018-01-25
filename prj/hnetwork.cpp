#include "hnetwork.h"
#include "hdsctx.h"
#include "hdsconf.h"

IMPL_SINGLETON(HNetwork)

#include <QNetworkConfigurationManager>

HNetwork::HNetwork() : QObject()
{
    QNetworkConfigurationManager netcfg;
    QList<QNetworkConfiguration> cfg_list = netcfg.allConfigurations();
    for (int i = 0; i < cfg_list.size(); ++i){
        QNetworkConfiguration cfg = cfg_list.at(i);
        qInfo("i=%d name=%s type=%s state=%d", i, cfg.name().toLocal8Bit().data(),
              cfg.bearerTypeName().toLocal8Bit().data(), cfg.state());
    }

    m_nam_post_screeninfo = new QNetworkAccessManager(this);

    m_nam_add_overlay = new QNetworkAccessManager(this);
    QObject::connect( m_nam_add_overlay, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryOverlayInfo()) );

    m_nam_remove_overlay = new QNetworkAccessManager(this);
    QObject::connect( m_nam_remove_overlay, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryOverlayInfo()) );

    m_nam_modify_overlay = new QNetworkAccessManager(this);
    QObject::connect( m_nam_modify_overlay, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryOverlayInfo()) );

    m_nam_query_overlay = new QNetworkAccessManager(this);
    QObject::connect( m_nam_query_overlay, SIGNAL(finished(QNetworkReply*)), this, SLOT(onQueryOverlayReply(QNetworkReply*)) );

    m_nam_micphone = new QNetworkAccessManager(this);

    m_nam_voice = new QNetworkAccessManager(this);
    QObject::connect( m_nam_voice, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryVoice()) );

    m_nam_query_voice = new QNetworkAccessManager(this);
    QObject::connect( m_nam_query_voice, SIGNAL(finished(QNetworkReply*)), this, SLOT(onQueryVoiceReply(QNetworkReply*)) );

    m_nam_post_notify = new QNetworkAccessManager(this);
}

void HNetwork::networkErrHandler(int errCode){
    // @bug: no good idea, so directly exitInstance.
    qWarning("err=%d", errCode);
    HNetwork::exitInstance();
}

void HNetwork::addItem(HAbstractItem* item){
    if (item->type == HAbstractItem::SCREEN){

    }else if (item->type == HAbstractItem::PICTURE){
        addPicture(*(HPictureItem*)item);
    }else if (item->type == HAbstractItem::TEXT){
        addText(*(HTextItem*)item);
    }
}

void HNetwork::removeItem(HAbstractItem* item){
    if (item->type == HAbstractItem::SCREEN){
        DsCombInfo si = g_dsCtx->m_tComb;
        if (si.items[item->id].srvid != 0){
            si.items[item->id].srvid = 0;
            si.items[item->id].a = false;
            postCombInfo(si);
        }
    }else if (item->type == HAbstractItem::PICTURE){
        removePicture(*(HPictureItem*)item);
    }else if (item->type == HAbstractItem::TEXT){
        removeText(*(HTextItem*)item);
    }
}

void HNetwork::modifyItem(HAbstractItem* item){
    if (item->type == HAbstractItem::SCREEN){
        DsCombInfo si = g_dsCtx->m_tComb;
        si.items[item->id].rc = item->rc;
        dsnetwork->postCombInfo(si);
    }else if (item->type == HAbstractItem::PICTURE){
        modifyPicture(*(HPictureItem*)item);
    }else if (item->type == HAbstractItem::TEXT){
        modifyText(*(HTextItem*)item);
    }
}

void HNetwork::queryItem(HAbstractItem* item){

}

void HNetwork::postCombInfo(DsCombInfo& si){
    QJsonArray arr;
    for (int i = 0; i < si.itemCnt; ++i){
        HCombItem& item = si.items[i];
        if (!item.v)
            continue;
        QJsonObject obj;
        obj.insert("x", item.rc.x());
        obj.insert("y", item.rc.y());
        obj.insert("w", item.rc.width());
        obj.insert("h", item.rc.height());
        obj.insert("v", OUTER_SRVID(item.srvid));
        obj.insert("a", item.a ? 1 : 0);
        arr.append(obj);
    }
    QJsonDocument doc;
    doc.setArray(arr);
    QByteArray bytes = doc.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/combinfo")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    QNetworkReply* reply = m_nam_post_screeninfo->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());

    if (reply->error() != QNetworkReply::NoError){
        qCritical("err=%d", reply->errorString().toLocal8Bit().data());
        networkErrHandler(reply->error());
    }
}

void HNetwork::queryOverlayInfo(){
    qInfo("%s", dsconf->value("URL/query_overlay").toLocal8Bit().data());
    QNetworkReply* reply = m_nam_query_overlay->get(QNetworkRequest(QUrl(dsconf->value("URL/query_overlay"))));

    if (reply->error() != QNetworkReply::NoError){
        qCritical("err=%d", reply->errorString().toLocal8Bit().data());
        networkErrHandler(reply->error());
    }
}

void HNetwork::onQueryOverlayReply(QNetworkReply* reply){
    QByteArray bytes = reply->readAll();
    qInfo(bytes.constData());

    QJsonParseError err;
    QJsonDocument dom = QJsonDocument::fromJson(bytes, &err);
    qDebug("err=%d,offset=%d",err.error, err.offset);
    QJsonObject obj_root;
    obj_root = dom.object();
    if (obj_root.contains("picture")){
        QJsonArray arr_picture = obj_root.value("picture").toArray();
        g_dsCtx->m_pics.itemCnt = qMin(arr_picture.size(), MAXNUM_PICTURE_ITEM);
        for (int i = 0; i < g_dsCtx->m_pics.itemCnt; ++i){
            QJsonObject obj_picture = arr_picture[i].toObject();

            HPictureItem* item = &g_dsCtx->m_pics.items[i];
            if (obj_picture.contains("id")){
                item->id = obj_picture.value("id").toString().toInt();
            }

            if (obj_picture.contains("x_pos") && obj_picture.contains("y_pos") &&
                obj_picture.contains("width") && obj_picture.contains("height")){
                int x = obj_picture.value("x_pos").toInt();
                int y = obj_picture.value("y_pos").toInt();
                int w = obj_picture.value("width").toInt();
                int h = obj_picture.value("height").toInt();
                item->rc.setRect(x,y,w,h);
            }

            if (obj_picture.contains("path")){
                QString src;
                src = dsconf->value("PATH/pic_local");
                src += obj_picture.value("path").toString();
                strncpy(item->src, src.toLocal8Bit().constData(), MAXLEN_STR);
            }

            qInfo("id=%d,x=%d,y=%d,w=%d,h=%d,src=%s", item->id, item->rc.x(), item->rc.y(), item->rc.width(), item->rc.height(),
                   item->src);
        }
    }

    if (obj_root.contains("text")){
        QJsonArray arr_text = obj_root.value("text").toArray();

        g_dsCtx->m_texts.itemCnt = qMin(arr_text.size(), MAXNUM_TEXT_ITEM);
        for (int i = 0; i < g_dsCtx->m_texts.itemCnt; ++i){
            QJsonObject obj_text = arr_text[i].toObject();
            HTextItem* item = &g_dsCtx->m_texts.items[i];
            if (obj_text.contains("id")){
                item->id = obj_text.value("id").toString().toInt();
            }

            if (obj_text.contains("x_pos") && obj_text.contains("y_pos") &&
                obj_text.contains("width") && obj_text.contains("height")){
                int x = obj_text.value("x_pos").toInt();
                int y = obj_text.value("y_pos").toInt();
                int w = obj_text.value("width").toInt();
                int h = obj_text.value("height").toInt();
                item->rc.setRect(x,y,w,h);
            }

            if (obj_text.contains("content")){
                QString text = obj_text.value("content").toString();
                strncpy(item->text, text.toLocal8Bit().constData(), MAXLEN_STR);
            }

            if (obj_text.contains("font_size")){
                item->font_size = obj_text.value("font_size").toInt();
            }

            if (obj_text.contains("font_color")){
                item->font_color = obj_text.value("font_color").toString().toInt(NULL, 16);
            }

            QFont font;
            font.setPixelSize(item->font_size);
            font.setLetterSpacing(QFont::AbsoluteSpacing, 0);
            font.setWordSpacing(0);
            QFontMetrics fm(font);
            int h = fm.height();
            int w = 256;
            QString text = item->text;
            if (text.contains("__%%TIMER%%__")){
                item->text_type = HTextItem::TIME;
                w = fm.width("2017-09-10 12:34:56");
            }else if (text.contains("__%%WATCHER%%__")){
                item->text_type = HTextItem::WATCHER;
                w = fm.width("00:00:00:0");
            }else if (text.contains("__%%subtitle_index%%__")){
                item->text_type = HTextItem::SUBTITLE;
                w = 360;
            }else{
                item->text_type = HTextItem::LABEL;
                font.setLetterSpacing(QFont::AbsoluteSpacing, 4);
                QFontMetrics fm(font);
                w = fm.width(item->text);
            }
            int x = item->rc.x();
            int y = g_dsCtx->m_tComb.height - item->rc.y() - h;//y_pos is from bottom
            item->rc.setRect(x,y,w,h);

            qInfo("id=%d,x=%d,y=%d,w=%d,h=%d,content=%s,font_size=%d,font_color=0x%x", item->id, item->rc.x(), item->rc.y(), item->rc.width(), item->rc.height(),
                   item->text,item->font_size, item->font_color);
        }
    }

    emit overlayChanged();

    reply->deleteLater();
}

void HNetwork::addPicture(HPictureItem &item){
    QJsonObject obj;
    QString src;
    if (item.pic_type == HPictureItem::IMAGE){
        src = item.src + dsconf->value("PATH/pic_local").size();
    }else if (item.pic_type == HPictureItem::MOSAIC){
        src = "Upload/mosaic.png";
        //src = "__%%MOSAIC%%__";
        obj.insert("pix", 20);
        obj.insert("transp", 50);
    }else if (item.pic_type == HPictureItem::BLUR){
        src = "Upload/blur.png";
        //src = "__%%BLUE%%__";
        obj.insert("transp", 50);
    }

    obj.insert("src", src);
    obj.insert("x", item.rc.x());
    obj.insert("y", item.rc.y());
    obj.insert("w", item.rc.width());
    obj.insert("h", item.rc.height());
    QJsonArray arr;
    arr.append(obj);
    QJsonObject obj_pic;
    obj_pic.insert("picture", arr);
    QJsonDocument dom;
    dom.setObject(obj_pic);
    QByteArray bytes = dom.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/add_overlay")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_add_overlay->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());
}

void HNetwork::addText(HTextItem& item){
    QJsonObject obj;
    obj.insert("content", item.text);
    obj.insert("x", item.rc.x());
    obj.insert("y", g_dsCtx->m_tComb.height - item.rc.bottom());
    obj.insert("font_size", item.font_size);
    char color[32];
    sprintf(color, "0x%x", item.font_color);
    obj.insert("font_color", color);
    QJsonArray arr;
    arr.append(obj);
    QJsonObject obj_pic;
    obj_pic.insert("text", arr);
    QJsonDocument dom;
    dom.setObject(obj_pic);
    QByteArray bytes = dom.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/add_overlay")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_add_overlay->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());
}

void HNetwork::modifyPicture(HPictureItem& item){
    QJsonObject obj;
    obj.insert("id", item.id);
    obj.insert("x", item.rc.x());
    obj.insert("y", item.rc.y());
    obj.insert("w", item.rc.width());
    obj.insert("h", item.rc.height());
    QJsonArray arr;
    arr.append(obj);
    QJsonObject obj_pic;
    obj_pic.insert("picture", arr);
    QJsonDocument dom;
    dom.setObject(obj_pic);
    QByteArray bytes = dom.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/modify_overlay")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_modify_overlay->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());
}

void HNetwork::modifyText(HTextItem& item){
    QJsonObject obj;
    obj.insert("id", item.id);
    obj.insert("x", item.rc.x());
    obj.insert("y", g_dsCtx->m_tComb.height - item.rc.bottom());
    obj.insert("font_size", item.font_size);
    char color[32];
    sprintf(color, "0x%x", item.font_color);
    obj.insert("font_color", color);
    QJsonArray arr;
    arr.append(obj);
    QJsonObject obj_pic;
    obj_pic.insert("text", arr);
    QJsonDocument dom;
    dom.setObject(obj_pic);
    QByteArray bytes = dom.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/modify_overlay")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_modify_overlay->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());
}

void HNetwork::removePicture(HPictureItem& item){
    QJsonObject obj;
    obj.insert("id", item.id);
    obj.insert("type", "picture");
    QJsonArray arr;
    arr.append(obj);
    QJsonDocument dom;
    dom.setArray(arr);
    QByteArray bytes = dom.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/remove_overlay")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_remove_overlay->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());
}

void HNetwork::removeText(HTextItem& item){
    QJsonObject obj;
    obj.insert("id", item.id);
    obj.insert("type", "text");
    QJsonArray arr;
    arr.append(obj);
    QJsonDocument dom;
    dom.setArray(arr);
    QByteArray bytes = dom.toJson();

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/remove_overlay")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_remove_overlay->post(req, bytes);

    qInfo() << req.url().url();
    qInfo(bytes.constData());
}

void HNetwork::setMicphone(int srvid){
    QString json = QString::asprintf("{\"id\":%d}", srvid);

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/micphone")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_micphone->post(req, json.toUtf8());

    qInfo() << req.url().url();
    qInfo(json.toUtf8().constData());
}

void HNetwork::queryVoice(){
    m_nam_query_voice->get(QNetworkRequest(QUrl(dsconf->value("URL/voiceinfo"))));
}

void HNetwork::onQueryVoiceReply(QNetworkReply *reply){
    QByteArray bytes = reply->readAll();
    qInfo(bytes.data());
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isArray()){
        QJsonArray arr = doc.array();
        for (int i = 0; i < arr.size(); ++i){
            QJsonObject obj = arr[i].toObject();
            int srvid,a;
            if (obj.contains("id")){
                srvid = obj.value("id").toInt();
            }

            if (obj.contains("a")){
                a = obj.value("a").toInt();
            }

            DsSrvItem* pItem = g_dsCtx->getSrvItem(srvid);
            if (pItem){
                pItem->bVoice = a;
            }
        }
    }

    emit g_dsCtx->voiceChanged();

    reply->deleteLater();
}

void HNetwork::setVoice(int srvid, int a){
    QString json = QString::asprintf("{\"id\":%d,\"a\":%d}", srvid, a);

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/voiceinfo")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_voice->post(req, json.toUtf8());

    qInfo() << req.url().url();
    qInfo(json.toUtf8().constData());
}

void HNetwork::notifyFullscreen(bool bFullscreen){
    QString json;
    if (bFullscreen){
        json = "{\"full_screen\":true}";
    }else{
        json = "{\"full_screen\":false}";
    }

    QNetworkRequest req;
    req.setUrl(QUrl(dsconf->value("URL/notify_fullscreen")));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    m_nam_post_notify->post(req, json.toUtf8());

    qInfo() << req.url().url();
    qInfo(json.toUtf8().constData());
}

