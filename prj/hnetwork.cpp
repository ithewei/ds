#include "hnetwork.h"

const char* url_post_combinfo = "http://localhost/transcoder/index.php?controller=channels&action=Dragsave";
const char* url_query_overlay = "http://localhost/transcoder/index.php?controller=logo&action=allinfo";
const char* url_add_overlay = "http://localhost/transcoder/index.php?controller=logo&action=logoadd";
const char* dir_trans = "/var/www/transcoder/";

HNetwork* HNetwork::s_pNetwork = NULL;

HNetwork::HNetwork() : QObject()
{
    m_nam_post_screeninfo = new QNetworkAccessManager(this);

    m_nam_add_overlay = new QNetworkAccessManager(this);
    QObject::connect( m_nam_add_overlay, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryOverlayInfo()) );

    m_nam_query_overlay = new QNetworkAccessManager(this);
    QObject::connect( m_nam_query_overlay, SIGNAL(finished(QNetworkReply*)), this, SLOT(onQueryOverlayReply(QNetworkReply*)) );
}

HNetwork* HNetwork::instance(){
    if (s_pNetwork == NULL){
        s_pNetwork = new HNetwork;
    }
    return s_pNetwork;
}

void HNetwork::exitInstance(){
    if (s_pNetwork){
        delete s_pNetwork;
        s_pNetwork = NULL;
    }
}

void HNetwork::postScreenInfo(DsScreenInfo si){
    QJsonArray arr;
    for (int i = 0; i < si.itemCnt; ++i){
        QJsonObject obj;
        ScreenItem& item = si.items[i];
        obj.insert("x", item.rc.x());
        obj.insert("y", item.rc.y());
        obj.insert("w", item.rc.width());
        obj.insert("h", item.rc.height());
        obj.insert("v", item.v);
        obj.insert("a", item.a ? 1 : 0);
        arr.append(obj);
    }
    QJsonDocument doc;
    doc.setArray(arr);
    QByteArray bytes = doc.toJson();
    qDebug(bytes.constData());
    m_nam_post_screeninfo->post(QNetworkRequest(QUrl(url_post_combinfo)), bytes);
}

void HNetwork::queryOverlayInfo(){
    m_nam_query_overlay->get(QNetworkRequest(QUrl(url_query_overlay)));
}

void HNetwork::onQueryOverlayReply(QNetworkReply* reply){
    QByteArray bytes = reply->readAll();
    qDebug(bytes.constData());

    QJsonParseError err;
    QJsonDocument dom = QJsonDocument::fromJson(bytes, &err);
    qDebug("err=%d,offset=%d",err.error, err.offset);
    QJsonObject obj_root;
    obj_root = dom.object();
    if (obj_root.contains("picture")){
        QJsonArray arr_picture = obj_root.value("picture").toArray();
        m_vecPictures.clear();
        for (int i = 0; i < arr_picture.size(); ++i){
            QJsonObject obj_picture = arr_picture[i].toObject();
            PictureItem item;
            if (obj_picture.contains("id")){
                item.id = obj_picture.value("id").toString().toInt();
            }

            if (obj_picture.contains("x_pos") && obj_picture.contains("y_pos") &&
                obj_picture.contains("width") && obj_picture.contains("height")){
                int x = obj_picture.value("x_pos").toInt();
                int y = obj_picture.value("y_pos").toInt();
                int w = obj_picture.value("width").toInt();
                int h = obj_picture.value("height").toInt();
                item.rc.setRect(x,y,w,h);
            }

            if (obj_picture.contains("path")){
                item.src = dir_trans;
                item.src += obj_picture.value("path").toString();
            }

            m_vecPictures.push_back(item);
            qDebug("id=%d,x=%d,y=%d,w=%d,h=%d,src=%s", item.id, item.rc.x(), item.rc.y(), item.rc.width(), item.rc.height(),
                   item.src.toLocal8Bit().constData());
        }
    }

    if (obj_root.contains("text")){
        QJsonArray arr_text = obj_root.value("text").toArray();
        m_vecTexts.clear();
        for (int i = 0; i < arr_text.size(); ++i){
            QJsonObject obj_text = arr_text[i].toObject();
            TextItem item;
            if (obj_text.contains("id")){
                item.id = obj_text.value("id").toInt();
            }

            if (obj_text.contains("x_pos") && obj_text.contains("y_pos") &&
                obj_text.contains("width") && obj_text.contains("height")){
                int x = obj_text.value("x_pos").toInt();
                int y = obj_text.value("y_pos").toInt();
                int w = obj_text.value("width").toInt();
                int h = obj_text.value("height").toInt();
                item.rc.setRect(x,y,w,h);
            }

            if (obj_text.contains("content")){
                item.text = obj_text.value("content").toString();
            }

            if (obj_text.contains("font_size")){
                item.font_size = obj_text.value("font_size").toInt();
            }

            if (obj_text.contains("font_color")){
                item.font_color = obj_text.value("font_color").toInt();
            }

            m_vecTexts.push_back(item);
            qDebug("id=%d,x=%d,y=%d,w=%d,h=%d,content=%s,font_size=%d", item.id, item.rc.x(), item.rc.y(), item.rc.width(), item.rc.height(),
                   item.text.toLocal8Bit().constData(),item.font_size);
        }
    }

    emit overlayChanged();

    reply->deleteLater();
}

void HNetwork::overlayPicture(PictureItem &item){
    QString src = item.src.right(item.src.length() - strlen(dir_trans));
    QJsonObject obj;
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
    qDebug(bytes.constData());
    m_nam_add_overlay->post(QNetworkRequest(QUrl(url_add_overlay)), bytes);
}

