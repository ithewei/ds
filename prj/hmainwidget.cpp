#include "hmainwidget.h"
#include "hrcloader.h"

HMainWidget::HMainWidget(HDsContext* ctx, QWidget *parent)
    : QMainWindow(parent)
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
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowTitle("Anystreaming Director");
    setFocus();

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
        HGLWidget* wdg;
        if (i == m_ctx->m_tLayout.itemCnt - 1){
            wdg = new HCombGLWidget(this);
            wdg->srvid = 1; // comb srvid = 1
        }else{
            wdg = new HGeneralGLWidget(this);
            wdg->srvid = 0;
        }
        wdg->wndid = i+1;
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

    QObject::connect( m_ctx, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)) );
    QObject::connect( m_ctx, SIGNAL(videoPushed(int,bool)), this, SLOT(onvideoPushed(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(audioPushed(int)), this, SLOT(onAudioPushed(int)) );
    QObject::connect( m_ctx, SIGNAL(sigStop(int)), this, SLOT(onStop(int)) );
    QObject::connect( m_ctx, SIGNAL(quit()), this, SLOT(hide()) );
    QObject::connect( m_ctx, SIGNAL(sigProgressNty(int,int)), this, SLOT(onProgressNty(int,int)) );
    //QObject::connect( m_ctx, SIGNAL(combChanged()), this, SLOT(getScreenInfo()) );

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

HGLWidget* HMainWidget::getGLWdgByWndid(int wndid){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        if (m_vecGLWdg[i]->wndid == wndid)
            return m_vecGLWdg[i];
    }

    return NULL;
}

HGLWidget* HMainWidget::getGLWdgBysrvid(int srvid){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];
        if (wdg->srvid == srvid)
            return wdg;
    }

    // not found, return min_wndid
    int min_wndid = 10000;
    HGLWidget* wdg = NULL;
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        if (m_vecGLWdg[i]->isResetStatus() && m_vecGLWdg[i]->isVisible() && m_vecGLWdg[i]->wndid < min_wndid){
            wdg = m_vecGLWdg[i];
            min_wndid = wdg->wndid;
        }
    }

    if (wdg){
        wdg->srvid = srvid;
        g_dsCtx->resizeForScale(srvid, wdg->width(), wdg->height());
    }

    return wdg;
}

HGLWidget* HMainWidget::getGLWdgByPos(QPoint pt){
    return getGLWdgByPos(pt.x(), pt.y());
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
            m_ctx->setAction(0);
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

    if (!wdg->isResetStatus() && !m_labelDrag->isVisible() && wdg->srvid != 1){
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
            if (wdg->srvid == 1){
                // pick comb's source
                HOperateTarget* target = ((HCombGLWidget*)wdg)->getItemByPos(QPoint(event->x()-wdg->x(), event->y()-wdg->y()), HAbstractItem::SCREEN);
                if (target)
                    changeScreenSource(target->pItem->id, m_dragSrcWdg->srvid);
            }else{
                // exchange srvid
                int srvid_src = m_dragSrcWdg->srvid;
                int srvid_dst = wdg->srvid;
                m_dragSrcWdg->resetStatus();
                wdg->resetStatus();
                m_dragSrcWdg->srvid = srvid_dst;
                wdg->srvid = srvid_src;
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
        DsSvrItem* item = m_ctx->getItem(wdg->srvid);
        if (item && !wdg->isResetStatus() && item->v_input != wdg->m_nPreFrame){
            wdg->repaint();
            item->bUpdateVideo = true;
        }   
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

void HMainWidget::onvideoPushed(int srvid, bool bFirstFrame){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL)
        return;

    bool bRepainter = false;
    if (m_ctx->display_mode == DISPLAY_MODE_REALTIME)
        bRepainter = true;

    wdg->setStatus(PLAYING | wdg->status(MINOR_STATUS_MASK) | PLAY_VIDEO, bRepainter);
}

void HMainWidget::onAudioPushed(int srvid){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL)
        return;

    // audio not repaint
    wdg->setStatus(PLAYING | wdg->status(MINOR_STATUS_MASK) | PLAY_AUDIO, false);
}

void HMainWidget::onStop(int srvid){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL)
        return;

    qDebug("");
    wdg->onStop();
}

void HMainWidget::onProgressNty(int srvid, int progress){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
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
}

void HMainWidget::onExitFullScreen(){
    QWidget* pSender = (QWidget*)sender();

    pSender->setWindowFlags(Qt::SubWindow);
    pSender->setGeometry(m_rcSavedGeometry);
    pSender->showNormal();
}

void HMainWidget::onGLWdgClicked(){
    HGLWidget* pSender = (HGLWidget*)sender();

    if (m_focusGLWdg == pSender){
        if (!m_focusGLWdg->showToolWidgets(false))
            m_focusGLWdg = NULL;
    }else{
        if (m_focusGLWdg){
            m_focusGLWdg->showToolWidgets(false);
        }
        if (pSender->showToolWidgets(true))
            m_focusGLWdg = pSender;
    }
}

void HMainWidget::changeScreenSource(int index, int srvid){
    DsScreenInfo si = m_ctx->m_tComb;
    if (si.items[index].srvid != srvid){
        si.items[index].srvid = srvid;
        if (srvid == 0){
            si.items[index].a = false;
        }
        HNetwork::instance()->postScreenInfo(si);
    }
}
