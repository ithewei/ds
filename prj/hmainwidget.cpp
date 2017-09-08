#include "hmainwidget.h"
#include "hrcloader.h"

const char* url_get_combinfo = "http://localhost/transcoder/index.php?controller=channels&action=Daoboinfo";
const char* url_post_combinfo = "http://localhost/transcoder/index.php?controller=channels&action=Dragsave";

HMainWidget::HMainWidget(HDsContext* ctx, QWidget *parent)
    : QWidget(parent)
{
    m_ctx = ctx;
    m_focusGLWdg = NULL;

    initUI();
    initConnect();
}

HMainWidget::~HMainWidget(){

}

void HMainWidget::initUI(){
    qDebug("");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    setWindowTitle("Anystreaming Director");
    if (m_ctx->m_tLayout.width == 0 || m_ctx->m_tLayout.height == 0){
       m_ctx->m_tLayout.width  = QApplication::desktop()->width();
       m_ctx->m_tLayout.height = QApplication::desktop()->height();
    }
    setGeometry(0,0,m_ctx->m_tLayout.width, m_ctx->m_tLayout.height);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::black);
    setPalette(pal);

    for (int i = 0; i < m_ctx->m_tLayout.itemCnt; ++i){
        // last is comb window,svrid = 1

        HGLWidget* wdg;
        if (i == m_ctx->m_tLayout.itemCnt - 1){
            wdg = new HCombGLWidget(this);
            wdg->svrid = 1;
            m_mapGLWdg[1] = wdg;
            QObject::connect( wdg, SIGNAL(combChanged(DsCombInfo)), this, SLOT(postCombInfo(DsCombInfo)) );
            QObject::connect( wdg, SIGNAL(undo()), this, SLOT(undo()) );
        }else{
            wdg = new HGeneralGLWidget(this);
            wdg->svrid = 0;
        }
        wdg->setGeometry(m_ctx->m_tLayout.items[i]);
        wdg->setTitleColor(m_ctx->m_tInit.titcolor);
        wdg->setOutlineColor(m_ctx->m_tInit.outlinecolor);
        m_vecGLWdg.push_back(wdg);
    }

    qDebug("screen_w=%d,screen_h=%d", width(), height());

    m_btnLeftExpand = new QPushButton(this);
    m_btnLeftExpand->setGeometry(width()-ICON_WIDTH-1, height()-ICON_HEIGHT-1, ICON_WIDTH, ICON_HEIGHT);
    m_btnLeftExpand->setIcon(QIcon(HRcLoader::instance()->icon_left_expand));
    m_btnLeftExpand->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnLeftExpand->setFlat(true);

    m_btnRightFold = new QPushButton(this);
    m_btnRightFold->setGeometry(width()-ICON_WIDTH-1, height()-ICON_HEIGHT-1, ICON_WIDTH, ICON_HEIGHT);
    m_btnRightFold->setIcon(QIcon(HRcLoader::instance()->icon_right_fold));
    m_btnRightFold->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnRightFold->setFlat(true);
    m_btnRightFold->hide();

    m_toolbar = new HMainToolbar(this);
    m_toolbar->setGeometry(0, height()-ICON_HEIGHT-1, width()-ICON_WIDTH, ICON_HEIGHT);
    m_toolbar->setAutoFillBackground(true);
    pal = m_toolbar->palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    m_toolbar->setPalette(pal);
    m_toolbar->hide();

    m_labelDrag = new QLabel(this);
    m_labelDrag->setStyleSheet("border:3px groove #FF8C00");
    m_labelDrag->hide();
}

void HMainWidget::initConnect(){
    qDebug("");
    setFocus(); // set key focus

    QObject::connect( m_ctx, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)) );
    QObject::connect( m_ctx, SIGNAL(videoPushed(int,bool)), this, SLOT(onvideoPushed(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(audioPushed(int)), this, SLOT(onAudioPushed(int)) );
    QObject::connect( m_ctx, SIGNAL(sigStop(int)), this, SLOT(onStop(int)) );
    QObject::connect( m_ctx, SIGNAL(quit()), this, SLOT(hide()) );
    QObject::connect( m_ctx, SIGNAL(sigProgressNty(int,int)), this, SLOT(onProgressNty(int,int)) );
    //QObject::connect( m_ctx, SIGNAL(combChanged()), this, SLOT(getCombInfo()) );

    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        QObject::connect( m_vecGLWdg[i], SIGNAL(fullScreen()), this, SLOT(onFullScreen()) );
        QObject::connect( m_vecGLWdg[i], SIGNAL(exitFullScreen()), this, SLOT(onExitFullScreen()) );
        QObject::connect( m_vecGLWdg[i], SIGNAL(clicked()), this, SLOT(onGLWdgClicked()) );
    }

    QObject::connect( m_btnLeftExpand, SIGNAL(clicked(bool)), this, SLOT(expand()) );
    QObject::connect( m_btnRightFold, SIGNAL(clicked(bool)), this, SLOT(fold()) );

    QObject::connect( &timer_repaint, SIGNAL(timeout()), this, SLOT(onTimerRepaint()) );
    if (m_ctx->display_mode == DISPLAY_MODE_TIMER){
        timer_repaint.start(1000 / m_ctx->frames);
        //timer_repaint.start(1000 / 20);
    }

    m_NAM_GetCombInfo = new QNetworkAccessManager(this);
    QObject::connect( m_NAM_GetCombInfo, SIGNAL(finished(QNetworkReply*)), this, SLOT(onGetCombInfoReply(QNetworkReply*)) );

    m_NAM_PostCombInfo = new QNetworkAccessManager(this);
    QObject::connect( m_NAM_PostCombInfo, SIGNAL(finished(QNetworkReply*)), this, SLOT(onPostCombInfoReply(QNetworkReply*)) );
}

HGLWidget* HMainWidget::getGLWdgBySvrid(int svrid){
    std::map<int, HGLWidget*>::iterator iter = m_mapGLWdg.find(svrid);
    if (iter != m_mapGLWdg.end()){
        HGLWidget* wdg = iter->second;
        if (wdg->svrid == svrid){
            return wdg;
        }
    }

    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        if (m_vecGLWdg[i]->svrid == 0){
            m_vecGLWdg[i]->svrid = svrid;
            m_mapGLWdg[svrid] = m_vecGLWdg[i];
            return m_vecGLWdg[i];
        }
    }

    return NULL;
}

HGLWidget* HMainWidget::getGLWdgByPos(int x, int y){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        QRect rc = m_vecGLWdg[i]->geometry();
        if (rc.contains(x,y)){
            return m_vecGLWdg[i];
        }
    }

    return NULL;
}

void HMainWidget::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
        case Qt::Key_Backspace:
        {
            m_ctx->setAction(0);
            event->accept();
        }
            break;
        case Qt::Key_Escape:
        {
            close();
            event->accept();
        }
            break;
        defualt:
            break;
    }

    QWidget::keyPressEvent(event);
}

void HMainWidget::mousePressEvent(QMouseEvent *event){

}

void HMainWidget::mouseMoveEvent(QMouseEvent *event){
    HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
    if (!wdg)
        return;

    if (wdg->status(MAJOR_STATUS_MASK) == PLAYING && !m_labelDrag->isVisible() && wdg->svrid != 1){
        m_dragSrcWdg = wdg;
        m_labelDrag->setPixmap( QPixmap::fromImage(wdg->grabFramebuffer()).scaled(DRAG_WIDTH, DRAG_HEIGHT) );
        m_labelDrag->show();
    }

    if (m_labelDrag->isVisible()){
        m_labelDrag->setGeometry(event->x()-DRAG_WIDTH/2, event->y()-DRAG_HEIGHT, DRAG_WIDTH,DRAG_HEIGHT);
    }
}

void HMainWidget::mouseReleaseEvent(QMouseEvent *event){
    // drag release
    if (m_labelDrag->isVisible()){
        m_labelDrag->hide();
        HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
        if (wdg == NULL)
            return;

        if (m_dragSrcWdg != wdg){
            if (wdg->svrid == 1){
                // pick comb's source
//                DsEvent evt;
//                evt.type = DS_EVENT_PICK;
//                evt.src_svrid = m_dragSrcWdg->svrid;
//                evt.dst_svrid = 1;
//                evt.dst_x = event->x() - wdg->x();
//                evt.dst_y = event->y() - wdg->y();
//                m_ctx->handle_event(evt);
                HCombGLWidget::TargetInfo target = ((HCombGLWidget*)wdg)->getTargetByPos(
                            QPoint(event->x()-wdg->x(), event->y()-wdg->y()), HCombGLWidget::SCREEN);
                changeCombSource(target.index, m_dragSrcWdg->svrid);
            }else{
                // exchange position
                QRect rcSrc = m_dragSrcWdg->geometry();
                QRect rcDst = wdg->geometry();
                wdg->setGeometry(rcSrc);
                m_dragSrcWdg->setGeometry(rcDst);
            }
        }
    }else{ // normal clicked
        HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
        if (wdg == NULL)
            return;
    }
}

void HMainWidget::onTimerRepaint(){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];
        DsSvrItem* item = m_ctx->getItem(wdg->svrid);
        if (wdg->status(MAJOR_STATUS_MASK) == PLAYING && item && item->v_input != wdg->m_nPreFrame){
            wdg->update();
        }
        if (item)
            item->bUpdateVideo = true;
    }
}

void HMainWidget::onActionChanged(int action){
    if (action == 0){
        hide();
    }else if (action == 1){
        showFullScreen();

        // when hide,status change but not repaint
        for (int i = 0; i < m_vecGLWdg.size(); ++i){
            m_vecGLWdg[i]->repaint();
        }
    }
}

void HMainWidget::onvideoPushed(int svrid, bool bFirstFrame){
    HGLWidget* wdg = getGLWdgBySvrid(svrid);
    if (wdg == NULL)
        return;

    bool bRepainter = false;
    if (m_ctx->display_mode == DISPLAY_MODE_REALTIME)
        bRepainter = true;

    wdg->setStatus(PLAYING | wdg->status(MINOR_STATUS_MASK) | PLAY_VIDEO, bRepainter);
}

void HMainWidget::onAudioPushed(int svrid){
    HGLWidget* wdg = getGLWdgBySvrid(svrid);
    if (wdg == NULL)
        return;

    // audio not repaint
    wdg->setStatus(PLAYING | wdg->status(MINOR_STATUS_MASK) | PLAY_AUDIO, false);
}

void HMainWidget::onStop(int svrid){
    HGLWidget* wdg = getGLWdgBySvrid(svrid);
    if (wdg == NULL)
        return;

    qDebug("");
    wdg->onStop();
}

void HMainWidget::onProgressNty(int svrid, int progress){
    HGLWidget* wdg = getGLWdgBySvrid(svrid);
    if (wdg == NULL)
        return;

    ((HGeneralGLWidget*)wdg)->setProgress(progress);
}

void HMainWidget::expand(){
    m_btnLeftExpand->hide();
    m_btnRightFold->show();
    m_toolbar->show();
}

void HMainWidget::fold(){
    m_btnLeftExpand->show();
    m_btnRightFold->hide();
    m_toolbar->hide();
}

void HMainWidget::onFullScreen(){
    QWidget* pSender = (QWidget*)sender();

    m_rcSavedGeometry = pSender->geometry();

    pSender->setWindowFlags(Qt::Window);
    pSender->showFullScreen();

    m_ctx->fullscreen(((HGLWidget*)pSender)->svrid, true);
}

void HMainWidget::onExitFullScreen(){
    QWidget* pSender = (QWidget*)sender();

    pSender->setWindowFlags(Qt::SubWindow);
    pSender->setGeometry(m_rcSavedGeometry);
    pSender->showNormal();

    m_ctx->fullscreen(((HGLWidget*)pSender)->svrid, false);
}

void HMainWidget::onGLWdgClicked(){
    HGLWidget* pSender = (HGLWidget*)sender();

    if (m_focusGLWdg == pSender){
        m_focusGLWdg->showToolWidgets(false);
        m_focusGLWdg = NULL;
    }else{
        if (m_focusGLWdg){
            m_focusGLWdg->showToolWidgets(false);
        }
        pSender->showToolWidgets(true);
        m_focusGLWdg = pSender;
    }
}

void HMainWidget::getCombInfo(){
    if (m_NAM_GetCombInfo)
        m_NAM_GetCombInfo->get(QNetworkRequest(QUrl(url_get_combinfo)));
}

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
void HMainWidget::onGetCombInfoReply(QNetworkReply* reply){
    QByteArray bytes = reply->readAll();
    qDebug(bytes.constData());
    QJsonDocument dom = QJsonDocument::fromJson(bytes);
    if (!dom.isNull()){
        if (dom.isArray()){
            QJsonArray arr = dom.array();
            for (int i = 0; i < arr.size(); ++i){
                QJsonValue val = arr[i];
                if (val.isObject()){
                    QJsonObject obj = val.toObject();
                    int iID = 0;
                    int iV = 0;
                    if (obj.contains("id")){
                        QJsonValue id = obj.value("id");
                        iID = id.toInt();
                    }
                    if (obj.contains("v")){
                        QJsonValue v = obj.value("v");
                        iV = v.toInt();
                    }
                    g_dsCtx->m_tComb.items[i].iSvrid = iID;
                    g_dsCtx->m_tComb.items[i].bAudio = iV;
                    qDebug("id=%d a=%d", iID, iV);
                }
            }
        }
    }

    reply->deleteLater();
}

void HMainWidget::postCombInfo(DsCombInfo ci){
    if (m_NAM_PostCombInfo){
        QJsonArray arr;
        for (int i = 0; i < ci.itemCnt; ++i){
            QJsonObject obj;
            DsCombItem* item = &ci.items[i];
            obj.insert("x", item->x);
            obj.insert("y", item->y);
            obj.insert("w", item->w);
            obj.insert("h", item->h);
            obj.insert("v", item->iSvrid);
            obj.insert("a", item->bAudio ? 1 : 0);
            arr.append(obj);
        }
        QJsonDocument doc;
        doc.setArray(arr);
        QByteArray bytes = doc.toJson();
        qDebug(bytes.constData());
        m_NAM_PostCombInfo->post(QNetworkRequest(QUrl(url_post_combinfo)), bytes);
    }
}

void HMainWidget::onPostCombInfoReply(QNetworkReply* reply){

}

void HMainWidget::undo(){
    postCombInfo(m_ctx->m_tCombUndo);
}

void HMainWidget::changeCombSource(int index, int svrid){
    DsCombInfo ci = m_ctx->m_tComb;
    if (ci.items[index].iSvrid != svrid){
        ci.items[index].iSvrid = svrid;
        if (svrid == 0){
            ci.items[index].bAudio = false;
        }
        postCombInfo(ci);
    }
}
