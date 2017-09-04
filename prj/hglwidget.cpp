#include "ds_global.h"
#include "hglwidget.h"
#include "hdsctx.h"
#include "hrcloader.h"
#include "ds.h"
#include <QTimer>
#include "hmainwidget.h"

HGLWidget::HGLWidget(QWidget *parent)
    : QGLWidgetImpl(parent)
{
    m_bDrawTitle = true;
    m_bDrawAudio = true;

    m_status = STOP;
    m_tmMousePressed = 0;

    initUI();
    initConnect();
}

HGLWidget::~HGLWidget(){

}

void HGLWidget::initUI(){
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(2);

    m_titlebar = new HTitlebarWidget;
    m_titlebar->setFixedHeight(TITLE_BAR_HEIGHT);
    m_titlebar->hide();
    vbox->addWidget(m_titlebar);

    vbox->addStretch();

    m_toolbar = new HToolbarWidget;
    m_toolbar->setFixedHeight(TOOL_BAR_HEIGHT);
    m_toolbar->hide();
    vbox->addWidget(m_toolbar);

    setLayout(vbox);

    m_snapshot = new QLabel(this);
    m_snapshot->setStyleSheet("border:5px double #ADFF2F");
    m_snapshot->hide();

    m_numSelector = new HNumSelectWidget;
    m_numSelector->setWindowFlags(Qt::Popup);
    m_numSelector->setAttribute(Qt::WA_TranslucentBackground, true);
    m_numSelector->hide();
}

void HGLWidget::initConnect(){
    QObject::connect( m_titlebar, SIGNAL(fullScreen()), this, SIGNAL(fullScreen()) );
    QObject::connect( m_titlebar, SIGNAL(exitFullScreen()), this, SIGNAL(exitFullScreen()) );
    QObject::connect( m_titlebar->m_btnSnapshot, SIGNAL(clicked(bool)), this, SLOT(snapshot()) );
    //QObject::connect( m_titlebar->m_btnStartRecord, SIGNAL(clicked(bool)), this, SLOT(startRecord()) );
    //QObject::connect( m_titlebar->m_btnStopRecord, SIGNAL(clicked(bool)), this, SLOT(stopRecord()) );
    QObject::connect( m_titlebar->m_btnNum, SIGNAL(clicked(bool)), this, SLOT(showNumSelector()) );

    QObject::connect( m_toolbar, SIGNAL(sigStart()), this, SLOT(onStart()) );
    QObject::connect( m_toolbar, SIGNAL(sigPause()), this, SLOT(onPause()) );
    QObject::connect( m_toolbar, SIGNAL(sigStop()), this, SLOT(onStop()) );
    QObject::connect( m_toolbar, SIGNAL(progressChanged(int)), this, SLOT(onProgressChanged(int)) );

    QObject::connect( m_numSelector, SIGNAL(numSelected(int)), this, SLOT(onNumSelected(int)) );
    QObject::connect( m_numSelector, SIGNAL(numCanceled(int)), this, SLOT(onNumCanceled(int)) );
}

void HGLWidget::showTitlebar(bool bShow){
    if (bShow){
        if ((m_status & MAJOR_STATUS_MASK) == PLAYING ||
            (m_status & MAJOR_STATUS_MASK) == PAUSE){
            DsSvrItem* item = g_dsCtx->getItem(svrid);
            if (item){
                m_titlebar->setTitle(item->title.c_str());
            }
            m_titlebar->show();
        }
    }else{
        m_titlebar->hide();
    }
}

void HGLWidget::showToolbar(bool bShow){
    if (bShow){
        if ((m_status & MAJOR_STATUS_MASK) == PLAYING ||
            (m_status & MAJOR_STATUS_MASK) == PAUSE){

            if ((m_status & MAJOR_STATUS_MASK) == PLAYING){
                m_toolbar->m_btnStart->hide();
                m_toolbar->m_btnPause->show();
            }
            if ((m_status & MAJOR_STATUS_MASK) == PAUSE){
                m_toolbar->m_btnStart->show();
                m_toolbar->m_btnPause->hide();
            }

            if (g_dsCtx->getItem(svrid)->src_type == SRC_TYPE_FILE){
                m_toolbar->m_slider->show();
                m_toolbar->show();
            }
            if (svrid == 1){
                m_toolbar->m_slider->hide();
                m_toolbar->show();
            }
        }
    }else{
        m_toolbar->hide();
    }
}

void HGLWidget::toggleTitlebar(){
    if (m_titlebar->isVisible()){
        showTitlebar(false);
    }else{
        showTitlebar(true);
    }
}

void HGLWidget::toggleToolbar(){
    if (m_toolbar->isVisible()){
        showToolbar(false);
    }else{
        showToolbar(true);
    }
}

void HGLWidget::toggleToolWidgets(){
    toggleTitlebar();
    toggleToolbar();
}

void HGLWidget::onStart(){
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item && item->ifcb){
        qDebug("svrid=%d startplay", svrid);
        //item->bPause = false;
        if (svrid == 1){
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_chr, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
        }else{
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
        }
    }

}

void HGLWidget::onPause(){    
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item && item->ifcb){
        qDebug("svrid=%d ifservice_callback::e_service_cb_pause", svrid);
        //item->bPause = true;
        if (svrid == 1){
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_chr, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
        }else{
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 1, NULL);
        }
    }
    setStatus(PAUSE | status(MINOR_STATUS_MASK));
}

void HGLWidget::onStop(){
    setStatus(STOP);
    m_mapIcons.clear();
    m_nPreFrame = 0;

    if (svrid != 1){// svrid=1 is cock,reserve
        svrid = 0;
    }
}

void HGLWidget::onProgressChanged(int progress){
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item && item->ifcb){
        qDebug("svrid=%d progress=%d ifservice_callback::e_service_cb_playratio", svrid, progress);
        item->ifcb->onservice_callback(ifservice_callback::e_service_cb_playratio, libchar(), 0, 0, progress, NULL);
    }
}

#include "hffmpeg.h"
#include <QDir>
#include <QDateTime>
void HGLWidget::snapshot(){
    //test snapshot
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item && item->tex_yuv.data){
        static const char* prefix = "/var/transcoder/snapshot/";
        QDir dir;
        if (!dir.exists(prefix)){
            dir.mkpath(prefix);
        }

        Texture* tex_yuv = &item->tex_yuv;
        uchar* rgb = (uchar*)malloc(tex_yuv->width * tex_yuv->height * 4);
        yuv2rgb32(tex_yuv->data, tex_yuv->width, tex_yuv->height, rgb);
        QImage img(rgb, tex_yuv->width, tex_yuv->height, QImage::Format_RGB32);

        QDateTime tm = QDateTime::currentDateTime();
        QString strTime = tm.toString("yyyyMMdd_hhmmss_zzz");
        QString strSavePath = prefix + strTime + ".jpg";
        qDebug(strSavePath.toLocal8Bit().data());
        img.save(strSavePath.toLocal8Bit().data());

        m_snapshot->setGeometry(width()/4, height()/4, width()/2, height()/2);
        m_snapshot->setPixmap(QPixmap::fromImage(img).scaled(width()/2,height()/2));
        m_snapshot->show();
        QTimer::singleShot(1000, m_snapshot, SLOT(hide()) );

        free(rgb);
    }
}

void HGLWidget::startRecord(){

}

void HGLWidget::stopRecord(){

}

void HGLWidget::onNumSelected(int num){
    //g_dsCtx->m_tCock.items[num - 1].iSvrid = svrid;
    g_dsCtx->m_preselect[num-1] = svrid;
}

void HGLWidget::onNumCanceled(int num){
    //g_dsCtx->m_tCock.items[num - 1].iSvrid = 0;
    g_dsCtx->m_preselect[num-1] = 0;
}

void HGLWidget::showNumSelector(){
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        if (g_dsCtx->m_preselect[i] == svrid){
            m_numSelector->m_numSelects[i]->hide();
            m_numSelector->m_numCancels[i]->show();
        }else{
            m_numSelector->m_numCancels[i]->hide();
            m_numSelector->m_numSelects[i]->show();
        }
    }
    int w = m_numSelector->width();
    int h = m_numSelector->height();
    m_numSelector->setGeometry((width()-w)/2+x(), (height()-h)/2+y(), w, h);
    m_numSelector->show();
}

void HGLWidget::mousePressEvent(QMouseEvent* event){
    m_tmMousePressed = event->timestamp();
    m_ptMousePressed = event->pos();
    event->ignore();
}

void HGLWidget::mouseReleaseEvent(QMouseEvent* event){
    if ((event->timestamp() - m_tmMousePressed < 200)){
        toggleToolWidgets();
        emit clicked();
    }

    event->ignore();
}

void HGLWidget::mouseMoveEvent(QMouseEvent* e){
    // add delay to prevent misopration
    if ((e->timestamp() - m_tmMousePressed < 200)){
        e->accept();
        return;
    }

    e->ignore();
}

void HGLWidget::addIcon(int type, int x, int y, int w, int h){
    m_mutex.lock();
    if (m_mapIcons.find(type) == m_mapIcons.end()){
        DrawInfo di;
        di.left = x;
        di.top = y;
        di.right = x+w;
        di.bottom = y+h;
        m_mapIcons[type] = di;
        update();
    }
    m_mutex.lock();
}

void HGLWidget::removeIcon(int type){
    m_mutex.lock();
    std::map<int,DrawInfo>::iterator iter = m_mapIcons.find(type);
    if (iter != m_mapIcons.end()){
        m_mapIcons.erase(iter);
    }
    m_mutex.unlock();
}

Texture* HGLWidget::getTexture(int type){
    switch(type){
    case HAVE_AUDIO:
        return &HRcLoader::instance()->tex_sound;
    }

    return NULL;
}

void HGLWidget::drawVideo(){
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item){
        item->mutex.lock();
        if (item->tex_yuv.data && item->tex_yuv.width > 0 && item->tex_yuv.height > 0){
            drawYUV(&item->tex_yuv);
            m_nPreFrame = item->v_input;
        }
        item->mutex.unlock();
    }
}

void HGLWidget::drawAudio(){
    // draw sound average
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item){
        DrawInfo di;
        if (item->a_channels > 1){
            di.left = width()-4 - AUDIO_WIDTH*2;
            di.top = height()-2 - AUDIO_HEIGHT;
            di.right = di.left + AUDIO_WIDTH;
            di.bottom = di.top + AUDIO_HEIGHT;
            di.color = 0x00FFFF80;
            drawRect(&di, 1, true);

            di.left += 1;
            di.right -= 1;
            di.bottom -= 1;
            di.top = di.bottom - item->a_average[1] * AUDIO_HEIGHT / 65536;
            di.color = 0xFFFF0080;
            drawRect(&di, 1, true);
        }

        di.left = width()-2 - AUDIO_WIDTH;
        di.top = height()-2 - AUDIO_HEIGHT;
        di.right = di.left + AUDIO_WIDTH;
        di.bottom = di.top + AUDIO_HEIGHT;
        di.color = 0x00FFFF80;
        drawRect(&di, 1, true);

        di.left += 1;
        di.right -= 1;
        di.bottom -= 1;
        di.top = di.bottom - item->a_average[0] * AUDIO_HEIGHT / 65536;
        di.color = 0xFFFF0080;
        drawRect(&di, 1, true);

        item->bUpdateAverage = true;
    }
}

void HGLWidget::drawSelectNum(){
    DrawInfo di;
    di.top = height()-48;
    di.bottom = height()-1;
    di.left = 1;
    di.right = 48;
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        if (g_dsCtx->m_preselect[i] == svrid){
            drawTex(&HRcLoader::instance()->tex_numr[i], &di);
            di.left += 48;
            di.right += 48;
            if (g_dsCtx->m_tCock.items[i].bAudio){
                // draw sound icon
                DrawInfo diAudio;
                Texture *tex = getTexture(HAVE_AUDIO);
                diAudio.right = width() - 1;
                diAudio.top = 1;
                diAudio.left = diAudio.right - 32 + 1;
                diAudio.bottom = diAudio.top + 32 - 1;
                drawTex(tex, &diAudio);
            }
        }
    }
}

void HGLWidget::drawIcon(){
    m_mutex.lock();
    std::map<int,DrawInfo>::iterator iter = m_mapIcons.begin();
    while (iter != m_mapIcons.end()){
        Texture *tex = getTexture(iter->first);
        DrawInfo di = iter->second;
        if (tex){
            drawTex(tex, &di);
        }
        ++iter;
    }
    m_mutex.unlock();
}

void HGLWidget::drawTitle(){
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item && item->title.length() > 0){
        DrawInfo di;
        di.left = 2;
        di.top = 2;
        di.color = m_titcolor;
        drawStr(g_dsCtx->m_pFont, item->title.c_str(), &di);
    }
}

void HGLWidget::drawOutline(){
    DrawInfo di;
    di.left = 0;
    di.top = 0;
    di.right = width() - 1;
    di.bottom = height() - 1;
    if (m_titlebar->isVisible()){
        di.color = g_dsCtx->m_tInit.focus_outlinecolor;
    }else{
        di.color = m_outlinecolor;
    }
    drawRect(&di, 3);
}

void HGLWidget::paintGL(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // drawVideo->drawAudio->drawSelectNum->drawTitle->drawOutline

    DrawInfo di;
    switch (m_status & MAJOR_STATUS_MASK) {
    case STOP:
        di.left = width()/2 - 50;
        di.top = height()/2 - 20;
        di.color = 0xFFFFFFFF;
        drawStr(g_dsCtx->m_pFont, "NO VIDEO!", &di);
        break;
    case NOSIGNAL:
        di.left = width()/2 - 50;
        di.top = height()/2 - 20;
        di.color = 0xFFFFFFFF;
        drawStr(g_dsCtx->m_pFont, "NO SIGNAL!", &di);
        break;
    case PAUSE:
    case PLAYING:
        if (m_status & PLAY_VIDEO){
            drawVideo();
        }

        if (m_status & PLAY_AUDIO){
            if (m_bDrawAudio){
                drawAudio();
            }
        }

        // draw select num
        drawSelectNum();
        break;
    }

    if (m_bDrawTitle){
        drawTitle();
    }

    drawOutline();
}

//===============================================================================

HCockGLWidget::HCockGLWidget(QWidget* parent)
    : HGLWidget(parent)
{
    m_cockoutlinecolor = 0xFFFFFFFF;
    m_cocktype = 0;

    m_titlebar->m_btnNum->hide();
    m_toolbar->m_btnUndo->show();

    m_labelDrag = new QLabel(this);
    m_labelDrag->setStyleSheet("border:3px groove #FF8C00");
    m_labelDrag->hide();

    m_labelResize = new QLabel(this);
    m_labelResize->setStyleSheet("border:3px groove #FF8C00");
    m_labelResize->hide();

    m_wdgTrash = new HChangeColorWidget(this);
    m_wdgTrash->setPixmap(HRcLoader::instance()->icon_trash);
    m_wdgTrash->hide();

    QObject::connect( g_dsCtx, SIGNAL(cockChanged()), this, SLOT(onCockChanged()) );
    QObject::connect( m_toolbar->m_btnUndo, SIGNAL(clicked(bool)), this, SIGNAL(undo()) );

    setAttribute(Qt::WA_AcceptTouchEvents);
}

HCockGLWidget::~HCockGLWidget(){

}

void HCockGLWidget::toggleToolWidgets(){
    HGLWidget::toggleToolWidgets();
    toggleTrash();
}

#define LOCATION_PADDING    32
int HCockGLWidget::getLocation(QPoint pt, QRect rc){
    int l = NotIn;
    if (rc.contains(pt)){
         if (pt.x() - rc.left() < LOCATION_PADDING)
             l |= Left;
         if (rc.right() - pt.x() < LOCATION_PADDING)
             l |= Right;
         if (pt.y() - rc.top() < LOCATION_PADDING)
             l |= Top;
         if (rc.bottom() - pt.y() < LOCATION_PADDING)
             l |= Bottom;
         return l == NotIn ? Center : l;
    }

    return NotIn;
}

int HCockGLWidget::getCockByPos(QPoint pt){
    int index = 0;
    for (int i = 1; i < m_vecCocks.size(); ++i){
        if (m_vecCocks[i].contains(pt)){
            index = i;
            break;
        }
    }

    return index;
}

void HCockGLWidget::adjustPos(QRect &rc){
    int x = rc.x();
    int y = rc.y();
    int w = rc.width();
    int h = rc.height();
    if (rc.left() < 0)
        x = 0;
    if (rc.right() >= width())
        x = width()-w;
    if (rc.top() < 0)
        y = 0;
    if (rc.bottom() >= height())
        y = height()-h;

    rc.setRect(x,y,w,h);
}

void HCockGLWidget::onCockChanged(){
    // scale
    double scale_x = (double)width() / (double)g_dsCtx->m_tCock.width;
    double scale_y = (double)height() / (double)g_dsCtx->m_tCock.height;
    m_vecCocks.clear();
    for (int i = 0; i < g_dsCtx->m_tCock.itemCnt; ++i){
        int x = g_dsCtx->m_tCock.items[i].x * scale_x + 0.5;
        int y = g_dsCtx->m_tCock.items[i].y * scale_y + 0.5;
        int w = g_dsCtx->m_tCock.items[i].w * scale_x + 0.5;
        int h = g_dsCtx->m_tCock.items[i].h * scale_y + 0.5;
        QRect rc(x,y,w,h);
        m_vecCocks.push_back(rc);
    }

    if (m_vecCocks[0].width() > width()-10 && m_vecCocks[0].height() > height()-10){
        m_cocktype = PIP;
    }else{
        m_cocktype = TILED;
    }
}

void HCockGLWidget::drawOutline(){
    DrawInfo di;
    di.left = 0;
    di.top = 0;
    di.right = width() - 1;
    di.bottom = height() - 1;
    di.color = m_outlinecolor;
    drawRect(&di, 3);
}

void HCockGLWidget::drawTaskInfo(){
    DrawInfo di;
    if (g_dsCtx->m_pFont){
        int oldSize = g_dsCtx->m_pFont->FaceSize();
        g_dsCtx->m_pFont->FaceSize(32);
        separator sept(g_dsCtx->m_strTaskInfo.c_str(), "\r\n");
        di.top = 10;
        di.left = 10;
        di.color = g_dsCtx->m_tInit.infcolor;
        for (int i = 0; i < sept.size(); ++i){
            drawStr(g_dsCtx->m_pFont, sept[i], &di);
            di.top += g_dsCtx->m_pFont->LineHeight() + 10;
        }
        g_dsCtx->m_pFont->FaceSize(oldSize);
    }
}

void HCockGLWidget::drawCockInfo(){
    DrawInfo di;
    for (int i = 0; i < m_vecCocks.size(); ++i){
        // draw cock NO.
        di.left = m_vecCocks[i].left() + 1;
        di.bottom = m_vecCocks[i].bottom() - 1;
        di.top = di.bottom - 48 + 1;
        di.right = di.left + 48 - 1;
        drawTex(&HRcLoader::instance()->tex_numr[i], &di);

        // draw cock outline
        di.left = m_vecCocks[i].left();
        di.top = m_vecCocks[i].top();
        di.right = m_vecCocks[i].right();
        di.bottom = m_vecCocks[i].bottom();
        di.color = m_cockoutlinecolor;
        drawRect(&di);
    }
}

void HCockGLWidget::paintGL(){
    if (g_dsCtx->m_tInit.info){
        m_bDrawTitle = true;
        m_bDrawAudio = true;
    }else{
        m_bDrawTitle = false;
        m_bDrawAudio = false;
    }
    HGLWidget::paintGL();

    if (g_dsCtx->m_tInit.info){
        drawTaskInfo();
        drawCockInfo();
    }

    // draw focused cock outline
    if (m_titlebar->isVisible()){
        DrawInfo di;
        di.left = m_vecCocks[m_indexCock].left();
        di.top = m_vecCocks[m_indexCock].top();
        di.right = m_vecCocks[m_indexCock].right();
        di.bottom = m_vecCocks[m_indexCock].bottom();
        di.color = g_dsCtx->m_tInit.focus_outlinecolor;
        drawRect(&di, 3);
    }
}

void HCockGLWidget::resizeEvent(QResizeEvent *e){
    onCockChanged();
    m_wdgTrash->setGeometry(width()-128-1, height()/2-64, 128, 128);

    HGLWidget::resizeEvent(e);
}

void HCockGLWidget::mousePressEvent(QMouseEvent* e){
    HGLWidget::mousePressEvent(e);

    m_indexCock = getCockByPos(e->pos());
    m_location = getLocation(e->pos(), m_vecCocks[m_indexCock]);
}

void HCockGLWidget::mouseMoveEvent(QMouseEvent *e){
    HGLWidget::mouseMoveEvent(e);
    if (e->isAccepted())
        return;

    if (!QRect(0,0,width(),height()).contains(e->pos())){
        e->accept();
        return;
    }

    if (m_location & Center){
        // move cock
        if (status(MAJOR_STATUS_MASK) == PLAYING && !m_labelDrag->isVisible()){
            QRect rc = m_vecCocks[m_indexCock];
            if (m_wdgTrash->isVisible()){
                m_labelDrag->setFixedSize(DRAG_WIDTH, DRAG_HEIGHT);
                m_labelDrag->setPixmap(grab(rc).scaled(DRAG_WIDTH, DRAG_HEIGHT));
            }else{
                if (m_indexCock == 0 && m_cocktype == PIP) // main cock can not move
                    return;

                m_labelDrag->setFixedSize(rc.size());
                m_labelDrag->setPixmap(grab(rc));
            }
            m_labelDrag->show();
        }

        if (m_labelDrag->isVisible()){
            int w = m_labelDrag->width();
            int h = m_labelDrag->height();
            QRect rc(e->x()-w/2, e->y()-h/2, w, h);

            if (m_wdgTrash->isVisible()){
                if (m_wdgTrash->geometry().contains(e->pos())){
                    m_wdgTrash->changeColor(QColor(255, 0, 0, 128));
                }else{
                    m_wdgTrash->changeColor(Qt::transparent);
                }
            }else{
                adjustPos(rc);
            }
            m_labelDrag->setGeometry(rc);
        }
    }else{
        // resize cock
        if (m_indexCock == 0 && m_cocktype == PIP) // main cock can not resize
            return;

        if (status(MAJOR_STATUS_MASK) == PLAYING && !m_labelResize->isVisible()){
            m_pixmapCock = grab(m_vecCocks[m_indexCock]);
            m_labelResize->setGeometry(m_vecCocks[m_indexCock]);
            m_labelResize->show();
        }
        if (m_labelResize->isVisible()){
            QRect rc = m_labelResize->geometry();
            if (m_location & Left){
                if (e->x() < rc.right() - 2*LOCATION_PADDING)
                    rc.setLeft(e->x());
            }else if (m_location & Right){
                if (e->x() > rc.left() + 2*LOCATION_PADDING)
                rc.setRight(e->x());
            }
            if (m_location & Top){
                if (e->y() < rc.bottom() - 2*LOCATION_PADDING)
                    rc.setTop(e->y());
            }else if (m_location & Bottom){
                if (e->y() > rc.top() + 2*LOCATION_PADDING)
                    rc.setBottom(e->y());
            }
            m_labelResize->setGeometry(rc);
            m_labelResize->setPixmap(m_pixmapCock.scaled(rc.size()));
        }
    }
}

void HCockGLWidget::mouseReleaseEvent(QMouseEvent *e){
    HGLWidget::mouseReleaseEvent(e);

    if (m_labelResize->isVisible()){
        m_labelResize->hide();
        // resize cock
        reposCock(m_indexCock, m_labelResize->geometry());
    }

    if (m_labelDrag->isVisible()){
        m_labelDrag->hide();

        if (m_wdgTrash->isVisible()){
            if (m_wdgTrash->geometry().contains(e->pos())){
                // stop cock
                stopCock(m_indexCock);
            }
        }else{
            // move cock
            reposCock(m_indexCock ,m_labelDrag->geometry());
        }
    }
}

void HCockGLWidget::reposCock(int index, QRect rc){
    if (rc == m_vecCocks[index])
        return;

    DsCockInfo ci = g_dsCtx->m_tCock;

    double scale_x = (double)g_dsCtx->m_tCock.width / (double)width();
    double scale_y = (double)g_dsCtx->m_tCock.height / (double)height();
    ci.items[index].x = rc.x() * scale_x;
    ci.items[index].y = rc.y() * scale_y;
    ci.items[index].w = rc.width() * scale_x;
    ci.items[index].h = rc.height() * scale_y;

    emit cockChanged(ci);
}

void HCockGLWidget::stopCock(int index){
//    DsEvent evt;
//    evt.type = DS_EVENT_STOP;
//    evt.dst_svrid = 1;
//    evt.dst_x = m_ptMousePressed.x();
//    evt.dst_y = m_ptMousePressed.y();
//    g_dsCtx->handle_event(evt);

    DsCockInfo ci = g_dsCtx->m_tCock;
    if (ci.items[m_indexCock].iSvrid != 0){
        ci.items[m_indexCock].iSvrid = 0;
        ci.items[m_indexCock].bAudio = false;
        emit cockChanged(ci);
    }
}
