#include <QApplication>
#include <QDesktopWidget>
#include "hmainwidget.h"
#include "ds.h"

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
        HGLWidget* wdg = new HGLWidget(this);
        if (i == m_ctx->m_cntItem - 1){
            wdg->svrid = 1;
        }else{
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

    m_strBack = m_ctx->img_path + "back.tga";
    m_strStart = m_ctx->img_path + "start.tga";
    m_strPause = m_ctx->img_path + "pause.tga";
    m_strBrower = m_ctx->img_path + "brower.tga";

    m_rcBack.setRect(sw - ICON_WIDTH, sh - ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT);
    m_rcStart.setRect(sw - ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT);
    m_rcPause = m_rcStart;
    m_rcBrower.setRect(0, sh - ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT);

    m_btnBack = new QPushButton(this);
    m_btnBack->setIcon(QPixmap(m_strBack.c_str()));
    m_btnBack->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnBack->setGeometry(m_rcBack);
    m_btnBack->setFlat(true);

    m_btnStart = new QPushButton(this);
    m_btnStart->setIcon(QPixmap(m_strStart.c_str()));
    m_btnStart->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnStart->setGeometry(m_rcStart);
    m_btnStart->setFlat(true);
    m_btnStart->hide();

    m_btnPause = new QPushButton(this);
    m_btnPause->setIcon(QPixmap(m_strPause.c_str()));
    m_btnPause->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnPause->setGeometry(m_rcPause);
    m_btnPause->setFlat(true);
    m_btnPause->show();

    m_btnBrower = new QPushButton(this);
    m_btnBrower->setIcon(QPixmap(m_strBrower.c_str()));
    m_btnBrower->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnBrower->setGeometry(m_rcBrower);
    m_btnBrower->setFlat(true);

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

    QObject::connect( m_btnBack, SIGNAL(clicked()), this, SLOT(onBack()) );
    QObject::connect( m_btnStart, SIGNAL(clicked()), this, SLOT(onStart()) );
    QObject::connect( m_btnPause, SIGNAL(clicked()), this, SLOT(onPause()) );
    QObject::connect( m_btnBrower, SIGNAL(clicked()), this, SLOT(openWeb()) );

    QObject::connect( m_ctx, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)) );
    QObject::connect( m_ctx, SIGNAL(titleChanged(int,std::string)), this, SLOT(onTitleChanged(int,std::string)) );
    QObject::connect( m_ctx, SIGNAL(videoPushed(int,bool)), this, SLOT(onvideoPushed(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(audioPushed(int)), this, SLOT(onAudioPushed(int)) );
    QObject::connect( m_ctx, SIGNAL(sourceChanged(int,bool)), this, SLOT(onSourceChanged(int,bool)) );
    QObject::connect( m_ctx, SIGNAL(sigStop(int)), this, SLOT(stop(int)) );
    QObject::connect( m_ctx, SIGNAL(quit()), this, SLOT(close()) );

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

#include <QKeyEvent>
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

#include <QMouseEvent>
void HMainWidget::mousePressEvent(QMouseEvent *event){
    qDebug("");
}

void HMainWidget::mouseMoveEvent(QMouseEvent *event){
    qDebug("");

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
    qDebug("");

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
        if (event->y() > height()-OPT_IMAGE_HEIGHT/2){
            openWeb();
            return;
        }

        HGLWidget* wdg = getGLWdgByPos(event->x(), event->y());
        if (!wdg)
            return;
        int svrid = wdg->svrid;

        timer_click.stop();
        timer_click.start(3000);

        if (m_iClickedSvrid != 0){
            m_mapGLWdg[m_iClickedSvrid]->removeIcon(PICK);
            m_mapGLWdg[m_iClickedSvrid]->removeIcon(PROHIBIT);
        }

        if (!m_bPicked){
            if (svrid == 1){
                wdg->addIcon(PROHIBIT, event->x() - wdg->x(), event->y() - wdg->y());
            }
            else{
                wdg->addIcon(PICK, event->x() - wdg->x(), event->y() - wdg->y());
                m_bPicked = true;
            }
            m_iClickedSvrid = svrid;
        }else{
            if (svrid == 1){
                // pick cock's source
                wdg->addIcon(CHANGING, event->x() - wdg->x(), event->y() - wdg->y());
                DsEvent evt;
                evt.type = DS_EVENT_PICK;
                evt.src_svrid = m_iClickedSvrid;
                evt.dst_svrid = 1;
                evt.dst_x = event->x() - wdg->x();
                evt.dst_y = event->y() - wdg->y();
                m_ctx->handle_event(evt);
            }else if (svrid == m_iClickedSvrid){
                // cancel pick
                wdg->removeIcon(PICK);
            }else{
                // exchange position
                HGLWidget* srcWdg = m_mapGLWdg[m_iClickedSvrid];
                HGLWidget* dstWdg = wdg;
                QRect rcSrc = srcWdg->geometry();
                QRect rcDst = dstWdg->geometry();
                dstWdg->setGeometry(rcSrc);
                srcWdg->setGeometry(rcDst);
            }
            m_bPicked = false;
            m_iClickedSvrid = 0;
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

void HMainWidget::onBack(){
    m_ctx->setAction(0);
}

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

#include <QGraphicsEffect>
void HMainWidget::openWeb(){
    m_webView = new QWebEngineView;
    m_webView->setWindowFlags(Qt::Popup);
    m_webView->setWindowOpacity(0.7);

    m_webView->load(QUrl("http://www.video4a.com/"));
    m_webView->setGeometry(width()/4,height()/4,width()/2,height()/2);
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
    HGLWidget* wdg = m_mapGLWdg[svrid];
    wdg->addIcon(HAVE_AUDIO, wdg->width()-32, 0, 32, 32);
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
