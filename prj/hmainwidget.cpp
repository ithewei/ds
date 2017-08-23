#include "hmainwidget.h"
#include "hrcloader.h"

HMainWidget::HMainWidget(HDsContext* ctx, QWidget *parent)
    : QWidget(parent)
{
    m_ctx = ctx;

    initUI();
    initConnect();
}

HMainWidget::~HMainWidget(){

}

void HMainWidget::initUI(){
    qDebug("");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    if (m_ctx->width == 0 || m_ctx->height == 0){
       m_ctx->width  = QApplication::desktop()->width();
       m_ctx->height = QApplication::desktop()->height();
    }
    setGeometry(0,0,m_ctx->width, m_ctx->height);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::black);
    setPalette(pal);

    for (int i = 0; i < m_ctx->m_cntItem; ++i){
        // last is cock window,svrid = 1

        HGLWidget* wdg;
        if (i == m_ctx->m_cntItem - 1){
            wdg = new HCockGLWidget(this);
            wdg->svrid = 1;
            m_mapGLWdg[1] = wdg;
        }else{
            wdg = new HGLWidget(this);
            wdg->svrid = 0;
        }
        wdg->setGeometry(m_ctx->m_rcItems[i]);
        wdg->setTitleColor(m_ctx->titcolor);
        wdg->setOutlineColor(m_ctx->outlinecolor);
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
    QObject::connect( m_ctx, SIGNAL(titleChanged(int)), this, SLOT(onTitleChanged(int)) );
    QObject::connect( m_ctx, SIGNAL(videoPushed(int,bool)), this, SLOT(onvideoPushed(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(audioPushed(int)), this, SLOT(onAudioPushed(int)) );
    QObject::connect( m_ctx, SIGNAL(sourceChanged(int,bool)), this, SLOT(onSourceChanged(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(sigStop(int)), this, SLOT(onStop(int)) );
    QObject::connect( m_ctx, SIGNAL(quit()), this, SLOT(hide()) );
    QObject::connect( m_ctx, SIGNAL(sigProgressNty(int,int)), this, SLOT(onProgressNty(int,int)) );

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
    if (wdg && wdg->status(MAJOR_STATUS_MASK) == PLAYING && !m_labelDrag->isVisible()){
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
                // pick cock's source
                DsEvent evt;
                evt.type = DS_EVENT_PICK;
                evt.src_svrid = m_dragSrcWdg->svrid;
                evt.dst_svrid = 1;
                evt.dst_x = event->x() - wdg->x();
                evt.dst_y = event->y() - wdg->y();
                m_ctx->handle_event(evt);
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
        DsItemInfo* item = m_ctx->getItem(wdg->svrid);
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

void HMainWidget::onTitleChanged(int svrid){
    qDebug("");

    HGLWidget* wdg = getGLWdgBySvrid(svrid);
    if (wdg == NULL)
        return;

    wdg->setTitle(m_ctx->getItem(svrid)->title.c_str());
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

void HMainWidget::onSourceChanged(int svrid, bool bSucceed){
    qDebug("");

    HGLWidget* wdg = getGLWdgBySvrid(svrid);
    if (wdg == NULL)
        return;

    wdg->removeIcon(CHANGING);
    if (!bSucceed){
        //
    }
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

    wdg->setProgress(progress);
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
    qDebug("");

    QWidget* pSender = (QWidget*)sender();

    m_rcSavedGeometry = pSender->geometry();

    pSender->setWindowFlags(Qt::Window);
    pSender->showFullScreen();
}

void HMainWidget::onExitFullScreen(){
    qDebug("");

    QWidget* pSender = (QWidget*)sender();

    pSender->setWindowFlags(Qt::SubWindow);
    pSender->setGeometry(m_rcSavedGeometry);
    pSender->showNormal();
}

void HMainWidget::onGLWdgClicked(){
    HGLWidget* pSender = (HGLWidget*)sender();

    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        if (m_vecGLWdg[i] != pSender){
            m_vecGLWdg[i]->showTitlebar(false);
            m_vecGLWdg[i]->showToolbar(false);
        }
    }
}
