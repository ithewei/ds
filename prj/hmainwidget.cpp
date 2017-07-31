#include "hmainwidget.h"

HMainWidget::HMainWidget(HDsContext* ctx, QWidget *parent) : QWidget(parent)
{
    m_ctx = ctx;
    m_bPicked = false;
    m_iClickedSvrid = 0;

    m_webView = NULL;
    m_dragWdg = NULL;

    initUI();
    initConnect();
}

HMainWidget::~HMainWidget(){

}

void HMainWidget::initUI(){
    qDebug("");
    int sw = m_ctx->width;
    int sh = m_ctx->height;
    bool bFS = false;
    if (sw == 0 || sh == 0){
       m_ctx->width = sw = QApplication::desktop()->width();
       m_ctx->height = sh = QApplication::desktop()->height();
       bFS = true;
    }
    setGeometry(0,0,sw, sh);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0,0,0));
    setPalette(pal);

    for (int i = 0; i < m_ctx->m_cntItem; ++i){
        // last is cock window,svrid = 1
        // init tilte = svrid

        HGLWidget* wdg;
        if (i == m_ctx->m_cntItem - 1){
            wdg = new HCockGLWidget(this);
            wdg->svrid = 1;
        }else{
            wdg = new HGLWidget(this);
            wdg->svrid = i + 2;
        }
        char szTitle[8];
        sprintf(szTitle, "%02d", wdg->svrid);
        wdg->setGeometry(m_ctx->m_rcItems[i]);
        wdg->setTitle(szTitle);
        wdg->setTitleColor(m_ctx->titcolor);
        wdg->setOutlineColor(m_ctx->outlinecolor);
        m_mapGLWdg[wdg->svrid] = wdg;
    }

    m_dragWdg = new HGLWidget(this);
    m_dragWdg->setOutlineColor(0x00FF00FF);
    m_dragWdg->hide();

    if (bFS){
        showFullScreen();
    }
}

void HMainWidget::initConnect(){
    qDebug("");
    setFocus(); // set key focus

    QObject::connect( m_ctx, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)) );
    QObject::connect( m_ctx, SIGNAL(titleChanged(int,std::string)), this, SLOT(onTitleChanged(int,std::string)) );
    QObject::connect( m_ctx, SIGNAL(videoPushed(int,bool)), this, SLOT(onvideoPushed(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(audioPushed(int)), this, SLOT(onAudioPushed(int)) );
    QObject::connect( m_ctx, SIGNAL(sourceChanged(int,bool)), this, SLOT(onSourceChanged(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(sigStop(int)), this, SLOT(stop(int)) );
    QObject::connect( m_ctx, SIGNAL(quit()), this, SLOT(close()) );

    std::map<int, HGLWidget*>::iterator iter = m_mapGLWdg.begin();
    while (iter != m_mapGLWdg.end()){
        HGLWidget* wdg = iter->second;
        QObject::connect( wdg, SIGNAL(fullScreen()), this, SLOT(onFullScreen()) );
        QObject::connect( wdg, SIGNAL(exitFullScreen()), this, SLOT(onExitFullScreen()) );
        ++iter;
    }

    timer_click.setSingleShot(true);
    QObject::connect( &timer_click, SIGNAL(timeout()), this, SLOT(clearOpt()) );
}

HGLWidget* HMainWidget::getGLWdgByPos(int x, int y){
    std::map<int, HGLWidget*>::iterator iter = m_mapGLWdg.begin();
    while (iter != m_mapGLWdg.end()){
        HGLWidget* wdg = iter->second;
        QRect rc = wdg->geometry();
        if (rc.contains(x,y)){
            return wdg;
        }
        ++iter;
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

void HMainWidget::clearOpt(){
    qDebug("");
    if (m_iClickedSvrid != 0){
        m_mapGLWdg[m_iClickedSvrid]->removeIcon(PICK);
        m_mapGLWdg[m_iClickedSvrid]->removeIcon(PROHIBIT);
    }
    m_bPicked = false;
    m_iClickedSvrid = 0;
}

void HMainWidget::mousePressEvent(QMouseEvent *event){
    qDebug("%d,%d", event->x(), event->y());
}

void HMainWidget::mouseMoveEvent(QMouseEvent *event){
    qDebug("%d,%d", event->x(), event->y());

    HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
    if (wdg && wdg->svrid != 1){// cock can not drag
        if (m_dragWdg->isVisible() == false){
            m_dragWdg->setVisible(true);
            m_dragWdg->svrid = wdg->svrid;
            m_dragWdg->setStatus(wdg->status());
            m_dragWdg->repaint();
        }
    }

    m_dragWdg->setGeometry(event->x()-DRAG_WIDTH/2, event->y()-DRAG_HEIGHT, DRAG_WIDTH,DRAG_HEIGHT);
}

void HMainWidget::mouseReleaseEvent(QMouseEvent *event){
    qDebug("%d,%d", event->x(), event->y());

    // drag release
    if (m_dragWdg->isVisible()){
        m_dragWdg->hide();

        HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
        if (wdg == NULL)
            return;

        if (m_dragWdg->svrid != wdg->svrid){
            HGLWidget* srcWdg = m_mapGLWdg[m_dragWdg->svrid];
            HGLWidget* dstWdg = m_mapGLWdg[wdg->svrid];
            if (wdg->svrid == 1){
                // pick cock's source
                DsEvent evt;
                evt.type = DS_EVENT_PICK;
                evt.src_svrid = m_dragWdg->svrid;
                evt.dst_svrid = 1;
                evt.dst_x = event->x()-dstWdg->x();
                evt.dst_y = event->y()-dstWdg->y();
                m_ctx->handle_event(evt);
            }else{
                // exchange position
                QRect rcSrc = srcWdg->geometry();
                QRect rcDst = dstWdg->geometry();
                dstWdg->setGeometry(rcSrc);
                srcWdg->setGeometry(rcDst);
            }
        }
    }else{ // normal clicked
        if (event->y() > height()- 32){
            openWeb();
            return;
        }
    }
}

void HMainWidget::mouseDoubleClickEvent(QMouseEvent *event){
    qDebug("");
    HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
    if (wdg == NULL)
        return;

    if (wdg->svrid == 1){
        DsEvent evt;
        evt.type = DS_EVENT_STOP;
        evt.dst_svrid = 1;
        evt.dst_x = event->x() - wdg->x();
        evt.dst_y = event->y() - wdg->y();
        m_ctx->handle_event(evt);
    }
}

/*
void HMainWidget::onStart(){
    m_btnStart->hide();
    m_btnPause->show();
    m_ctx->setPause(0);
}

void HMainWidget::onPause(){
    m_btnStart->show();
    m_btnPause->hide();
    if(m_ctx->ifcb[0])
    {
        qDebug("");
        m_ctx->ifcb[0]->onservice_callback(ifservice_callback::e_service_cb_chr, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
    }
    m_mapGLWdg[1]->setStatus(PAUSE);
    m_ctx->setPause(1);
}
*/

#include <QGraphicsEffect>
void HMainWidget::openWeb(){
    m_webView = new QWebEngineView;
    m_webView->setWindowFlags(Qt::Popup);
    m_webView->setWindowOpacity(0.8);

    m_webView->load(QUrl("http://www.video4a.com/"));
    m_webView->setGeometry(x(), y()+height()-64, width(), 64);
    m_webView->show();
}

void HMainWidget::onActionChanged(int action){
    if (action == 0){
        hide();
    }else if (action == 1){
        show();
    }
}

void HMainWidget::onTitleChanged(int svrid, std::string title){
    m_mapGLWdg[svrid]->setTitle(title.c_str());
}

void HMainWidget::onvideoPushed(int svrid, bool bFirstFrame){
   m_mapGLWdg[svrid]->setStatus(PLAYING);
}

void HMainWidget::onAudioPushed(int svrid){
    if (svrid != 1){
        HGLWidget* wdg = m_mapGLWdg[svrid];
        wdg->addIcon(HAVE_AUDIO, wdg->width()-32, 0, 32, 32);
    }
}

void HMainWidget::onSourceChanged(int svrid, bool bSucceed){
    qDebug("");

    m_mapGLWdg[svrid]->removeIcon(CHANGING);
    if (!bSucceed){
        m_mapGLWdg[svrid]->setStatus(NOSIGNAL);
    }
}

void HMainWidget::stop(int svrid){
    m_mapGLWdg[svrid]->setStatus(STOP);
}

void HMainWidget::onFullScreen(){
    qDebug("");

    QWidget* pSender = (QWidget*)sender();

    m_rcSavedGeometry = pSender->geometry();

    QRect rc = pSender->geometry();
    qDebug("aaa:%d,%d", rc.width(),rc.height());

    pSender->setWindowFlags(Qt::Window);
    pSender->showFullScreen();

    rc = pSender->geometry();
    qDebug("bbb:%d,%d", rc.width(),rc.height());
}

void HMainWidget::onExitFullScreen(){
    qDebug("");

    QWidget* pSender = (QWidget*)sender();

    QRect rc = pSender->geometry();
    qDebug("ccc:%d,%d", rc.width(),rc.height());

    pSender->setWindowFlags(Qt::SubWindow);
    pSender->setGeometry(m_rcSavedGeometry);
    pSender->showNormal();

    rc = pSender->geometry();
    qDebug("ddd:%d,%d", rc.width(),rc.height());
}
