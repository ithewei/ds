#include "hmainwidget.h"
#include "hrcloader.h"

HMainWidget* g_mainWdg = NULL;

HMainWidget::HMainWidget(QWidget *parent) : HWidget(parent){
    m_focusGLWdg = NULL;
    m_fullscreenGLWdg = NULL;
    m_bMouseMoving = false;
    m_eOperate = EXCHANGE;

    initUI();
    initConnect();
}

#include <QScreen>
void HMainWidget::initUI(){
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    setWindowTitle("Anystreaming Director");
    setFocus();

    if (g_dsCtx->m_tInit.mouse)
        setMouseTracking(true);

    setBgFg(this, Qt::black, Qt::white);

    QScreen *screen = QApplication::primaryScreen();
    qInfo("w=%fmm h=%fmm dpi=%f", screen->physicalSize().width(), screen->physicalSize().height(), screen->physicalDotsPerInch());
    qInfo("resolution=%d*%d logicdpi=%f", screen->size().width(), screen->size().height(), screen->logicalDotsPerInch());
    qInfo("dpix=%f dpiy=%f", screen->physicalDotsPerInchX(), screen->physicalDotsPerInchY());

    QDesktopWidget* desktop = QApplication::desktop();
    qInfo("screenCount=%d, w=%d h=%d", desktop->screenCount(), desktop->width(), desktop->height());
    //setGeometry(0,0,g_dsCtx->m_tLayout.width, g_dsCtx->m_tLayout.height);
    // show in last screen
    setGeometry(desktop->screenGeometry(0));
    qInfo("x=%d y=%d screen_w=%d screen_h=%d", x(), y(), width(), height());
    if (g_dsCtx->m_tInit.autolayout){
        g_dsCtx->m_tLayout.width  = width();
        g_dsCtx->m_tLayout.height = height();
    }

    m_extGLWdg = NULL;
    setExtScreen(desktop->screenCount());

    HSaveInfo::instance()->read();
    if (HSaveInfo::instance()->wnd_num != 0){
        for (int i = 0; i < HSaveInfo::instance()->wnd_num; ++i){
            WndInfo* wnd =  &HSaveInfo::instance()->wndinfo[i];
            HGLWidget* wdg;
            if (wnd->type == HGLWidget::GENERAL){
                wdg = new HGeneralGLWidget(this);
            }else if (wnd->type == HGLWidget::COMB){
                wdg = new HCombGLWidget(this);
            }
            wdg->wndid = wnd->wndid;
            wdg->setGeometry(wnd->rc);
            wdg->setVisible(wnd->visible);
            m_vecGLWdg.push_back(wdg);
        }
    }else if (g_dsCtx->m_tInit.autolayout){
        for (int i = 0; i < g_dsCtx->m_tInit.maxnum_layout; ++i){
            int wndid = i+1;
            HGLWidget* wdg;
            if (wndid == g_dsCtx->m_tInit.output){
                wdg = new HCombGLWidget(this);
            }else{
                wdg = new HGeneralGLWidget(this);
            }
            wdg->wndid = wndid;
            m_vecGLWdg.push_back(wdg);
            wdg->hide();
        }

        if (g_dsCtx->m_tInit.row == 0 || g_dsCtx->m_tInit.col == 0){
            g_dsCtx->m_tInit.row = 3;
            g_dsCtx->m_tInit.col = 3;
        }
        m_layout.init(g_dsCtx->m_tInit.row, g_dsCtx->m_tInit.col);
        if (g_dsCtx->m_tInit.merge[0] && g_dsCtx->m_tInit.merge[1]){
            m_layout.merge(g_dsCtx->m_tInit.merge[0], g_dsCtx->m_tInit.merge[1]);
        }
        updateGLWdgsByLayout();
    }else{
        for (int i = 0; i < g_dsCtx->m_tLayout.itemCnt; ++i){
            int wndid = i+1;
            HGLWidget* wdg;
            if (wndid == g_dsCtx->m_tInit.output){
                wdg = new HCombGLWidget(this);
            }else{
                wdg = new HGeneralGLWidget(this);
            }
            wdg->wndid = wndid;
            wdg->setGeometry(g_dsCtx->m_tLayout.items[i]);
            m_vecGLWdg.push_back(wdg);
        }
    }

#if LAYOUT_TYPE_MULTI_INPUT
    m_toolbar = new HStyleToolbar(this);
    m_toolbar->setWindowFlags(Qt::Popup);
    m_toolbar->setAttribute(Qt::WA_TranslucentBackground, true);
    m_toolbar->setGeometry(0, height()-MAIN_TOOBAR_HEIGHT, width(), MAIN_TOOBAR_HEIGHT);
    m_toolbar->hide();
#endif

#if LAYOUT_TYPE_OUTPUT_AND_INPUT
    QSize sz(ICON_WIDTH, ICON_HEIGHT);
    m_btnLeftExpand = genPushButton(sz, rcloader->get(RC_LEFT_EXPAND), this);
    m_btnLeftExpand->setGeometry(width()-ICON_WIDTH-1, height()-ICON_HEIGHT-1, ICON_WIDTH, ICON_HEIGHT);

    m_btnRightFold = genPushButton(sz, rcloader->get(RC_RIGHT_FOLD), this);
    m_btnRightFold->setGeometry(width()-ICON_WIDTH-1, height()-ICON_HEIGHT-1, ICON_WIDTH, ICON_HEIGHT);
    m_btnRightFold->hide();

    m_toolbar = new HWebToolbar(this);
    //m_toolbar = new HModelToolbar(this);
    m_toolbar->setGeometry(0, height()-ICON_HEIGHT-1, width()-ICON_WIDTH, ICON_HEIGHT);
    setBgFg(m_toolbar, Qt::transparent);
    m_toolbar->hide();
#endif

    m_labelDrag = new QLabel(this);
    m_labelDrag->setStyleSheet("border:3px groove #FF8C00");
    m_labelDrag->hide();

    m_labelRect = new QLabel(this);
    m_labelRect->setStyleSheet("border:3px groove #FF8C00; background-color: rgba(0,0,0,0);");
    m_labelRect->hide();
}

#include <QFileSystemWatcher>
void HMainWidget::initConnect(){
    QObject::connect( g_dsCtx, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)) );
    QObject::connect( g_dsCtx, SIGNAL(requestShow(int)), this, SLOT(onRequestShow(int)) );
    QObject::connect( g_dsCtx, SIGNAL(videoPushed(int,bool)), this, SLOT(onvideoPushed(int,bool)) );
    QObject::connect( g_dsCtx, SIGNAL(audioPushed(int)), this, SLOT(onAudioPushed(int)) );
    QObject::connect( g_dsCtx, SIGNAL(sigStop(int)), this, SLOT(onStop(int)) );
    QObject::connect( g_dsCtx, SIGNAL(quit()), this, SLOT(hide()) );
    QObject::connect( g_dsCtx, SIGNAL(sigProgressNty(int,int)), this, SLOT(onProgressNty(int,int)) );
    QObject::connect( g_dsCtx, SIGNAL(combChanged()), this, SLOT(onCombChanged()) );
    QObject::connect( g_dsCtx, SIGNAL(voiceChanged()), this, SLOT(onVoiceChanged()) );
    QObject::connect( QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(onScreenCountChanged(int)) );
    QObject::connect( QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(onScreenResized(int)) );

    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];
        QObject::connect( wdg, SIGNAL(fullScreen(bool)), this, SLOT(onFullScreen(bool)) );
        QObject::connect( wdg, SIGNAL(clicked()), this, SLOT(onGLWdgClicked()) );
    }

#if LAYOUT_TYPE_OUTPUT_AND_INPUT
    QObject::connect( m_btnLeftExpand, SIGNAL(clicked(bool)), this, SLOT(expand()) );
    QObject::connect( m_btnRightFold, SIGNAL(clicked(bool)), this, SLOT(fold()) );
#endif

#if LAYOUT_TYPE_MULTI_INPUT
    QObject::connect( m_toolbar, SIGNAL(styleChanged(int,int)), this, SLOT(setLayout(int,int)) );
    QObject::connect( m_toolbar->m_btnMerge, SIGNAL(clicked(bool)), this, SLOT(onMerge()) );
#endif

    QObject::connect( &timer_repaint, SIGNAL(timeout()), this, SLOT(onTimerRepaint()) );
    if (g_dsCtx->m_tInit.display_mode == DISPLAY_MODE_TIMER){
        timer_repaint.start(1000 / g_dsCtx->m_tInit.fps);
    }


    if (g_dsCtx->m_tInit.save_span > 0){
        QObject::connect( &timer_save, SIGNAL(timeout()), this, SLOT(onTimerSave()) );
        timer_save.start(g_dsCtx->m_tInit.save_span*1000);
    }

    QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
    watcher->addPath(g_dsCtx->ds_path.c_str());
    QObject::connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)) );
    QObject::connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirChanged(QString)) );
}

HGLWidget* HMainWidget::getGLWdgByWndid(int wndid){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        if (m_vecGLWdg[i]->wndid == wndid)
            return m_vecGLWdg[i];
    }

    return NULL;
}

HGLWidget* HMainWidget::allocGLWdgForsrvid(int srvid){
    // getGLWdgBysrvid -- HSaveInfo -- m_vecGLWdg -- create
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg)
        return wdg;

    for (int i = 0; i < HSaveInfo::instance()->wnd_num; ++i){
        if (HSaveInfo::instance()->wndinfo[i].srvid == srvid){
            HGLWidget* p = getGLWdgByWndid(HSaveInfo::instance()->wndinfo[i].wndid);
            if (p && p->isResetStatus() && p->isVisible()){
                wdg = p;
            }
            break;
        }
    }

    if (!wdg){
        int min_wndid = 10000;
        for (int i = 0; i < m_vecGLWdg.size(); ++i){
            if (isOutputSrvid(srvid)){
                if (m_vecGLWdg[i]->type != HGLWidget::COMB)
                    continue;
            }else{
                if (m_vecGLWdg[i]->type != HGLWidget::GENERAL)
                    continue;
            }
            if (m_vecGLWdg[i]->isResetStatus() && m_vecGLWdg[i]->isVisible() && m_vecGLWdg[i]->wndid < min_wndid){
                wdg = m_vecGLWdg[i];
                min_wndid = wdg->wndid;
            }
        }
    }

#if LAYOUT_TYPE_ONLY_OUTPUT
    if (!wdg){
        if (isLmic(srvid)){
            DsSrvItem* item = g_dsCtx->getSrvItem(srvid);
            if (item && item->pic_w > 0 && item->pic_h > 0){
                qInfo("allocGLWdgForLimc srvid=%d", srvid);
                HCombGLWidget* comb = (HCombGLWidget*)getGLWdgBysrvid(OUTPUT_SRVID);
                if (comb){
                    wdg = comb->allocGLWdgForLimc();
                    if (wdg){
                        wdg->srvid = srvid;
                        QRect rc = wdg->geometry();
                        double wnd_ratio = rc.width() / (double)rc.height();
                        double pic_ratio = item->pic_w / (double)item->pic_h;
                        double ratio = pic_ratio / wnd_ratio;
                        int w = rc.width();
                        int h = rc.height();
                        if (ratio > 1.0){
                            h /= ratio;
                        }else{
                            w *= ratio;
                        }

                        int x = rc.left();
                        int y = rc.top();
                        QPoint ptParent = comb->rect().center();
                        QPoint pt = rc.center();
                        if (pt.x() > ptParent.x())
                            x = rc.right() - w;
                        if (pt.y() > ptParent.y())
                            y = rc.bottom() - h;
                        wdg->setGeometry(x, y, w, h);
                        qInfo("x=%d y=%d w=%d h=%d", x, y, w, h);
                        m_vecGLWdg.push_back(wdg);
                        wdg->setVisible(true);
                    }
                }
            }
        }
    }
#endif

    if (wdg){
        wdg->srvid = srvid;
        qInfo("srvid:%d ==> wndid:%d", wdg->srvid, wdg->wndid);
    }

    return wdg;
}

HGLWidget* HMainWidget::getGLWdgBysrvid(int srvid){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];
        if (wdg->srvid == srvid)
            return wdg;
    }

    return NULL;
}

HGLWidget* HMainWidget::getGLWdgByPos(QPoint pt){
    return getGLWdgByPos(pt.x(), pt.y());
}

HGLWidget* HMainWidget::getGLWdgByPos(int x, int y){
    for (int i = m_vecGLWdg.size() - 1; i >= 0; --i){
        if (m_vecGLWdg[i]->isVisible()){
            QRect rc = m_vecGLWdg[i]->geometry();
            if (rc.contains(x,y)){
                return m_vecGLWdg[i];
            }
        }
    }

    return NULL;
}

void HMainWidget::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
        case Qt::Key_Backspace:
        case Qt::Key_Escape:
        {
            g_dsCtx->setAction(0);
            event->accept();
        }
            break;
        defualt:
            break;
    }

    QWidget::keyPressEvent(event);
}

void HMainWidget::mousePressEvent(QMouseEvent *event){
    m_ptMousePressed = event->pos();
}

void HMainWidget::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() == Qt::NoButton){
#if LAYOUT_TYPE_ONLY_OUTPUT
        // LAYOUT_TYPE_ONLY_OUTPUT no toolbar
#else
        if (event->y() > height() - MAIN_TOOBAR_HEIGHT && !m_toolbar->isVisible()){
                m_toolbar->show();
        }
#endif
    }else{
        HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
        if (!wdg)
            return;

        if (wdg->type == HGLWidget::LMIC){
            QRect rc = adjustPos(QRect(event->x()-wdg->width()/2, event->y()-wdg->height()/2, wdg->width(), wdg->height()), rect());
            wdg->setGeometry(rc);
            return;
        }

        if (!m_bMouseMoving){            
            m_bMouseMoving = true;
            // move begin

            m_dragSrcWdg = wdg;

            if (m_eOperate == EXCHANGE){
                if (wdg->type == HGLWidget::COMB)
                    return;

                if (!wdg->isResetStatus()){
                    m_labelDrag->setPixmap( QPixmap::fromImage(wdg->grabFramebuffer()).scaled(DRAG_WIDTH, DRAG_HEIGHT) );
                    m_labelDrag->show();
                    setCursor(QCursor(Qt::DragMoveCursor));
                }
            }else if (m_eOperate == MERGE){
                m_labelRect->show();
            }
        }

        if (m_labelRect->isVisible()){
            QRect rc;
            rc.setTopLeft(QPoint(qMin(m_ptMousePressed.x(), event->x()), qMin(m_ptMousePressed.y(), event->y())));
            rc.setBottomRight(QPoint(qMax(m_ptMousePressed.x(), event->x()), qMax(m_ptMousePressed.y(), event->y())));
            m_labelRect->setGeometry(rc);
        }

        if (m_labelDrag->isVisible()){
            m_labelDrag->setGeometry(event->x()-DRAG_WIDTH/2, event->y()-DRAG_HEIGHT, DRAG_WIDTH,DRAG_HEIGHT);
        }
    }
}

void HMainWidget::mouseReleaseEvent(QMouseEvent *event){
    if (m_bMouseMoving){
        m_bMouseMoving = false;
        setCursor(QCursor(Qt::ArrowCursor));
        // move end
        if (m_eOperate == EXCHANGE){
            if (!m_labelDrag->isVisible())
                return;
            m_labelDrag->hide();

            HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
            if (wdg && m_dragSrcWdg != wdg){
                if (wdg->type == HGLWidget::COMB){
                    // changeCombItem
                    HOperateObject obj = ((HCombGLWidget*)wdg)->getObejctByPos(QPoint(event->x()-wdg->x(), event->y()-wdg->y()), HAbstractItem::SCREEN);
                    if (obj.isNull()){
                        HCombItem item;
                        item.srvid = m_dragSrcWdg->srvid;
                        item.rc.setWidth(g_dsCtx->m_tComb.width);
                        item.rc.setHeight(g_dsCtx->m_tComb.height);
                        item.add();
                    }
                    else{
                        HCombItem* item = (HCombItem*)obj.pItem;
                        item->srvid = m_dragSrcWdg->srvid;
                        item->modify();
                    }
                }else if (wdg->type == HGLWidget::GENERAL){
                    // exchange wndid for order
                    std::swap(m_dragSrcWdg->wndid, wdg->wndid);
                    // exchange pos
                    QRect rcSrc = m_dragSrcWdg->geometry();
                    QRect rcDst = wdg->geometry();
                    m_dragSrcWdg->setGeometry(rcDst);
                    wdg->setGeometry(rcSrc);
                    qDebug("exchange srvid:%d ==> wndid:%d && srvid:%d ==> wndid:%d",
                           m_dragSrcWdg->srvid, m_dragSrcWdg->wndid,
                           wdg->srvid, wdg->wndid);
                }
            }
        }else if (m_eOperate == MERGE){
            if (!m_labelRect->isVisible())
                return;
            m_labelRect->hide();
            m_eOperate = EXCHANGE;
            HGLWidget* ltWdg = getGLWdgByPos(m_labelRect->geometry().topLeft());
            HGLWidget* rbWdg = getGLWdgByPos(m_labelRect->geometry().bottomRight());
            if (ltWdg && rbWdg && ltWdg != rbWdg){
                mergeGLWdg(ltWdg->wndid, rbWdg->wndid);
            }
        }
    }else{
        // click release
    }
}

void HMainWidget::onTimerRepaint(){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];

        // when fullscreen, just update m_fullscreenGLWdg
        if (m_fullscreenGLWdg && wdg != m_fullscreenGLWdg)
            continue;

        // pop_video
        bool bPopSucceed = false;
        bool bUpdateWdg = !wdg->isResetStatus() && wdg->isVisible();
        bool bUpdateExt = wdg->srvid == OUTPUT_SRVID && m_extGLWdg != NULL && m_extGLWdg->isVisible();
        if (bUpdateWdg || bUpdateExt){
            DsSrvItem* item = g_dsCtx->getSrvItem(wdg->srvid);
            // adjust video and audio sync
            if (item && item->bAudio && item->audio_player && !item->audio_player->pause &&
                    item->a_cur_ts != 0 && item->v_cur_ts != 0 &&
                    item->src_type != SRC_TYPE_LMIC){ // lmic span don't well
                qDebug("a_cur_ts=%u a_base_ts=%u v_cur_ts=%u v_base_ts=%u", item->a_cur_ts, item->a_base_ts, item->v_cur_ts, item->v_base_ts);
                long long a_span = item->a_cur_ts - item->a_base_ts;
                if (a_span < 0)
                    a_span = a_span + UINT_MAX;
                long long v_span = item->v_cur_ts - item->v_base_ts;
                if (v_span < 0)
                    v_span = v_span + UINT_MAX;
                if (v_span > a_span + g_dsCtx->m_tInit.av_maxspan){
                    qDebug("srvid=%d video faster audio, v_span=%ld, a_span=%ld", wdg->srvid, v_span, a_span);
                    if (item->src_type != SRC_TYPE_FILE) // file back maybe cause v_span faster audio
                        continue;
                }else if (v_span < a_span - g_dsCtx->m_tInit.av_maxspan){
                    qDebug("srvid=%d video slower audio, v_span=%ld, a_span=%ld", wdg->srvid, v_span, a_span);
                    g_dsCtx->discard_video(wdg->srvid, 1);
                }else{
                    qDebug("srvid=%d video approximate audio, v_span=%ld, a_span=%ld", wdg->srvid, v_span, a_span);
                }
            }

            bPopSucceed = (g_dsCtx->pop_video(wdg->srvid) == 0);
        }

        if (bUpdateWdg && bPopSucceed){
            wdg->update();
        }

        if (bUpdateExt && bPopSucceed){
            m_extGLWdg->update();
        }
    }
}

void HMainWidget::onActionChanged(int action){
    qInfo("mainwdg::action=%d", action);
    if (action == 0){
        hide();
    }else if (action == 1){
        hide();
        showFullScreen();
        raise();
        if (g_dsCtx->m_tInit.output != 0){
            dsnetwork->queryOverlayInfo();
#if LAYOUT_TYPE_OUTPUT_AND_INPUT
            if (g_dsCtx->m_tComb.itemCnt != 0){
                dsnetwork->postCombInfo(g_dsCtx->m_tComb);// for refresh combinfo
            }
            dsnetwork->queryVoice();
#endif
            HItemUndo::instance()->clear();
        }

        // when hide,status change but not repaint
        for (int i = 0; i < m_vecGLWdg.size(); ++i){
            m_vecGLWdg[i]->update();
            //m_vecGLWdg[i]->updateToolWidgets();
            m_vecGLWdg[i]->showToolWidgets(false);
        }
#if LAYOUT_TYPE_OUTPUT_AND_INPUT
        if (m_toolbar->isVisible())
            m_toolbar->show();
#endif
    }

#if LAYOUT_TYPE_ONLY_OUTPUT
    dsnetwork->notifyFullscreen(action);
#endif

    g_dsCtx->action = action; // sync action
}

void HMainWidget::onRequestShow(int srvid){
    qDebug("onRequestShow=%d", srvid);
    HGLWidget* wdg = allocGLWdgForsrvid(srvid);
    if (wdg == NULL)
        return;

    g_dsCtx->onRequestShowSucceed(srvid, wdg->videoArea());
}

void HMainWidget::onvideoPushed(int srvid, bool bFirstFrame){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL || bFirstFrame){
        onRequestShow(srvid);
        return;
    }

    bool bRepainter = false;
    if (g_dsCtx->m_tInit.display_mode == DISPLAY_MODE_REALTIME){
        g_dsCtx->pop_video(srvid);
        bRepainter = true;
    }

    wdg->setStatus(PLAYING | wdg->status(MINOR_STATUS_MASK) | PLAY_VIDEO, bRepainter);
}

void HMainWidget::onAudioPushed(int srvid){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL){
        return;
    }

    // audio not repaint
    wdg->setStatus(PLAYING | wdg->status(MINOR_STATUS_MASK) | PLAY_AUDIO, false);
}

void HMainWidget::onStop(int srvid){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL)
        return;

    wdg->onStop();


#if LAYOUT_TYPE_ONLY_OUTPUT
    if (isLmic(srvid)){
        wdg->hide();
        std::vector<HGLWidget*>::iterator iter = m_vecGLWdg.begin();
        while (iter != m_vecGLWdg.end()){
            if (*iter == wdg){
                m_vecGLWdg.erase(iter);
                break;
            }
            iter++;
        }
    }
#endif
}

void HMainWidget::onProgressNty(int srvid, int progress){
    HGLWidget* wdg = getGLWdgBysrvid(srvid);
    if (wdg == NULL)
        return;

    if (wdg->type == HGLWidget::GENERAL){
        ((HGeneralGLWidget*)wdg)->setProgress(progress);
    }
}

#if LAYOUT_TYPE_OUTPUT_AND_INPUT
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
#endif

void HMainWidget::onFullScreen(bool  bFullScreen){
#if LAYOUT_TYPE_ONLY_OUTPUT
    if (!bFullScreen){
        g_dsCtx->setAction(0);
    }
#else
    HGLWidget* pSender = (HGLWidget*)sender();

    if (bFullScreen){
        m_rcSavedGeometry = pSender->geometry();
        qInfo() << m_rcSavedGeometry;
        pSender->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        pSender->setGeometry(rect());
        pSender->showFullScreen();
#if LAYOUT_TYPE_MULTI_INPUT
#else
        hide();
#endif

        m_fullscreenGLWdg = pSender;

        DsSrvItem* pItem = g_dsCtx->getSrvItem(pSender->srvid);
        if (pItem && g_dsCtx->m_tInit.fps != pItem->framerate){
            timer_repaint.stop();
            timer_repaint.start(1000/pItem->framerate);
        }
    }else{
        pSender->setWindowFlags(Qt::Widget);
        pSender->setGeometry(m_rcSavedGeometry);
        pSender->show();
        show();

        m_fullscreenGLWdg = NULL;

        DsSrvItem* pItem = g_dsCtx->getSrvItem(pSender->srvid);
        if (pItem && g_dsCtx->m_tInit.fps != pItem->framerate){
            timer_repaint.stop();
            timer_repaint.start(1000/g_dsCtx->m_tInit.fps);
        }
    }
#endif
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

void HMainWidget::onMerge(){
    if (m_eOperate != MERGE){
        m_eOperate = MERGE;
        setCursor(QCursor(Qt::CrossCursor));
    }
    else
        m_eOperate = EXCHANGE;
}

void HMainWidget::setLayout(int row, int col){
    qInfo("setLayout %d*%d", row, col);
    m_layout.init(row, col);
    updateGLWdgsByLayout();
}

void HMainWidget::updateGLWdgsByLayout(){
    int w = width();
    int h = height();
    int col = m_layout.col;
    int row = m_layout.row;
    int cell_w = w / col;
    int cell_h = h / row;
    int margin_x = (w - cell_w*col) / 2;
    int margin_y = (h - cell_h*row) / 2;
    qDebug("sw=%d, sh=%d, cell_w=%d, cell_h=%d", w, h, cell_w, cell_h);

    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];
        HLayoutCell cell;
        if (m_layout.getLayoutCell(wdg->wndid, cell)){
            wdg->setGeometry(margin_x + (cell.c1-1)*cell_w, margin_y + (cell.r1-1)*cell_h, cell.getColspan()*cell_w, cell.getRowspan()*cell_h);
            wdg->show();
            wdg->update();
        }else{
            wdg->hide();
        }
    }
}

void HMainWidget::mergeGLWdg(int lt, int rb){
    qDebug("merge:%d ~ %d", lt, rb);
    HLayoutCell cell = m_layout.merge(lt, rb);
    qDebug("%d~%d %d~%d", cell.r1, cell.r2, cell.c1, cell.c2);
    // select first to merge, other reset.
    int select_srvid = 0;
    for (int r = cell.r1; r <= cell.r2; ++r)
        for (int c = cell.c1; c <= cell.c2; ++c){
            int id = (r-1)*m_layout.col + c;
            HGLWidget* wdg = getGLWdgByWndid(id);
            if (wdg && !wdg->isResetStatus()){
                if (select_srvid == 0)
                    select_srvid = wdg->srvid;
                wdg->resetStatus();
            }
        }

    if (select_srvid != 0)
        getGLWdgByWndid(lt)->srvid = select_srvid;

    updateGLWdgsByLayout();
}

void HMainWidget::updateAllToolWidgets(){
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        m_vecGLWdg[i]->updateToolWidgets();
    }
}

void HMainWidget::onVoiceChanged(){
    updateAllToolWidgets();
}

void HMainWidget::onCombChanged(){
    updateAllToolWidgets();

    // adjust micphone
    HCombItem* ci = g_dsCtx->getCombItem(g_dsCtx->m_tComb.micphone);
    if (g_dsCtx->m_tComb.micphone != 0 && ci && ci->v){
        dsnetwork->setMicphone(0);
    }
    if (g_dsCtx->pre_micphone_srvid != 0){
        DsSrvItem* item = g_dsCtx->getSrvItem(g_dsCtx->pre_micphone_srvid);
        HCombItem* ci = g_dsCtx->getCombItem(g_dsCtx->pre_micphone_srvid);
        if (item && item->bVoice && !ci){
            dsnetwork->setMicphone(g_dsCtx->pre_micphone_srvid);
        }
    }
}

void HMainWidget::onTimerSave(){
    if (!isVisible())
        return;

    if (m_fullscreenGLWdg)
        return;

    HSaveInfo::instance()->wnd_num = m_vecGLWdg.size();
    for (int i = 0; i < m_vecGLWdg.size(); ++i){
        HGLWidget* wdg = m_vecGLWdg[i];
        if (wdg){
            WndInfo* wnd = &HSaveInfo::instance()->wndinfo[i];
            wnd->type = wdg->type;
            wnd->wndid = wdg->wndid;
            wnd->srvid = wdg->srvid;
            wnd->visible = wdg->isVisible();
            wnd->rc = wdg->geometry();
        }
    }
    HSaveInfo::instance()->write();
}

void HMainWidget::onFileChanged(QString file){
    g_dsCtx->parse_layout_xml(g_dsCtx->layout_file.c_str());
}

void HMainWidget::onDirChanged(QString file){
    g_dsCtx->parse_layout_xml(g_dsCtx->layout_file.c_str());
}

void HMainWidget::setExtScreen(int cnt){
    qInfo("cnt=%d", cnt);
    if (cnt > 1){
        g_dsCtx->ext_screen = true;
        if (!m_extGLWdg){
            m_extGLWdg = new HGLWidget(this);
            m_extGLWdg->type = HGLWidget::EXTEND;
            m_extGLWdg->m_bDrawInfo = false;
            m_extGLWdg->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
            m_extGLWdg->wndid = 0;
            m_extGLWdg->srvid = OUTPUT_SRVID;
            m_extGLWdg->setGeometry(QApplication::desktop()->screenGeometry(1));
            m_extGLWdg->setStatus(PLAYING | PLAY_VIDEO);
            qInfo("m_extGLWdg: x=%d y=%d screen_w=%d,screen_h=%d", m_extGLWdg->x(), m_extGLWdg->y(), m_extGLWdg->width(), m_extGLWdg->height());
        }
        m_extGLWdg->showFullScreen();
    }else{
        g_dsCtx->ext_screen = false;
        if (m_extGLWdg){
            m_extGLWdg->hide();
        }
    }
}

void HMainWidget::onScreenCountChanged(int cnt){
    qInfo("cnt=%d", cnt);
    setExtScreen(cnt);

    DsSrvItem *item = g_dsCtx->getSrvItem(OUTPUT_SRVID);
    if (item && item->audio_player){
        item->audio_player->stopPlay();
    }
}

void HMainWidget::onScreenResized(int screen){
    if (screen == 1 && m_extGLWdg){
        m_extGLWdg->setGeometry(QApplication::desktop()->screenGeometry(1));
    }
}
