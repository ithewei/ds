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
    m_bShowTools = false;

    m_status = STOP;
    m_tmMousePressed = 0;

    m_snapshot = new QLabel(this);
    m_snapshot->setStyleSheet("border:5px double #ADFF2F");
    m_snapshot->hide();
}

HGLWidget::~HGLWidget(){

}

void HGLWidget::showToolWidgets(bool bShow){

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

    if (svrid != 1){// svrid=1 is comb,reserve
        svrid = 0;
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

void HGLWidget::mousePressEvent(QMouseEvent* event){
    m_tmMousePressed = event->timestamp();
    m_ptMousePressed = event->pos();
    event->ignore();
}

void HGLWidget::mouseReleaseEvent(QMouseEvent* event){
    if ((event->timestamp() - m_tmMousePressed < 200)){
        emit clicked();
    }

    event->ignore();
}

void HGLWidget::mouseMoveEvent(QMouseEvent* e){
    // add delay to prevent misopration
    if ((e->timestamp() - m_tmMousePressed < 100)){
        e->accept();
        return;
    }

    e->ignore();
}

void HGLWidget::addIcon(int type, int x, int y, int w, int h){
    if (m_mapIcons.find(type) == m_mapIcons.end()){
        DrawInfo di;
        di.left = x;
        di.top = y;
        di.right = x+w;
        di.bottom = y+h;
        m_mapIcons[type] = di;
        update();
    }
}

void HGLWidget::removeIcon(int type){
    std::map<int,DrawInfo>::iterator iter = m_mapIcons.find(type);
    if (iter != m_mapIcons.end()){
        m_mapIcons.erase(iter);
    }
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

void HGLWidget::drawIcon(){
    std::map<int,DrawInfo>::iterator iter = m_mapIcons.begin();
    while (iter != m_mapIcons.end()){
        Texture *tex = getTexture(iter->first);
        DrawInfo di = iter->second;
        if (tex){
            drawTex(tex, &di);
        }
        ++iter;
    }
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
    di.color = m_outlinecolor;
    drawRect(&di, 3);
}

void HGLWidget::paintGL(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // drawVideo->drawAudio->drawTitle->drawOutline

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
        break;
    }

    if (m_bDrawTitle){
        drawTitle();
    }

    drawOutline();
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HGeneralGLWidget::HGeneralGLWidget(QWidget* parent)
    : HGLWidget(parent)
{
    initUI();
    initConnect();
}

HGeneralGLWidget::~HGeneralGLWidget(){

}

void HGeneralGLWidget::initUI(){
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

    m_numSelector = new HNumSelectWidget(this);
    m_numSelector->setWindowFlags(Qt::Popup);
    m_numSelector->setAttribute(Qt::WA_TranslucentBackground, true);
}

void HGeneralGLWidget::initConnect(){
    QObject::connect( m_titlebar->m_btnFullScreen, SIGNAL(clicked(bool)), this, SIGNAL(fullScreen()) );
    QObject::connect( m_titlebar->m_btnExitFullScreen, SIGNAL(clicked(bool)), this, SIGNAL(exitFullScreen()) );
    QObject::connect( m_titlebar->m_btnSnapshot, SIGNAL(clicked(bool)), this, SLOT(snapshot()) );
    //QObject::connect( m_titlebar->m_btnStartRecord, SIGNAL(clicked(bool)), this, SLOT(startRecord()) );
    //QObject::connect( m_titlebar->m_btnStopRecord, SIGNAL(clicked(bool)), this, SLOT(stopRecord()) );
    QObject::connect( m_titlebar->m_btnNum, SIGNAL(clicked(bool)), this, SLOT(showNumSelector()) );

    QObject::connect( m_toolbar->m_btnStart, SIGNAL(clicked(bool)), this, SLOT(onStart()) );
    QObject::connect( m_toolbar->m_btnPause, SIGNAL(clicked(bool)), this, SLOT(onPause()) );
    QObject::connect( m_toolbar, SIGNAL(progressChanged(int)), this, SLOT(onProgressChanged(int)) );

    QObject::connect( m_numSelector, SIGNAL(numSelected(int)), this, SLOT(onNumSelected(int)) );
    QObject::connect( m_numSelector, SIGNAL(numCanceled(int)), this, SLOT(onNumCanceled(int)) );
}

void HGeneralGLWidget::showTitlebar(bool bShow){
    if (bShow){
        DsSvrItem* item = g_dsCtx->getItem(svrid);
        if (item){
            m_titlebar->m_label->setText(item->title.c_str());
        }
        m_titlebar->show();
    }else{
        m_titlebar->hide();
    }
}

void HGeneralGLWidget::showToolbar(bool bShow){
    if (bShow){
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
        }else{
            m_toolbar->m_slider->hide();
        }
            m_toolbar->show();
    }else{
        m_toolbar->hide();
    }
}

void HGeneralGLWidget::showToolWidgets(bool bShow){
    HGLWidget::showToolWidgets(bShow);

    if (bShow){
        if (!(status(MAJOR_STATUS_MASK) == PAUSE || status(MAJOR_STATUS_MASK) == PLAYING))
            return;
    }

    showTitlebar(bShow);
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item){
        if (g_dsCtx->getItem(svrid)->src_type == SRC_TYPE_FILE){
            showToolbar(bShow);
        }
    }
}

void HGeneralGLWidget::onNumSelected(int num){
    g_dsCtx->m_preselect[num-1] = svrid;
}

void HGeneralGLWidget::onNumCanceled(int num){
    g_dsCtx->m_preselect[num-1] = 0;
}

void HGeneralGLWidget::showNumSelector(){
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

void HGeneralGLWidget::onProgressChanged(int progress){
    DsSvrItem* item = g_dsCtx->getItem(svrid);
    if (item && item->ifcb){
        qDebug("svrid=%d progress=%d ifservice_callback::e_service_cb_playratio", svrid, progress);
        item->ifcb->onservice_callback(ifservice_callback::e_service_cb_playratio, libchar(), 0, 0, progress, NULL);
    }
}

void HGeneralGLWidget::drawSelectNum(){
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
        }
    }
}

void HGeneralGLWidget::drawSound(){
    for (int i = 0; i < g_dsCtx->m_tComb.itemCnt; ++i){
        if (g_dsCtx->m_tComb.items[i].iSvrid == svrid && g_dsCtx->m_tComb.items[i].bAudio){
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

void HGeneralGLWidget::drawOutline(){
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

void HGeneralGLWidget::paintGL(){
    HGLWidget::paintGL();

    if (status(MAJOR_STATUS_MASK) == PLAYING || status(MAJOR_STATUS_MASK) == PAUSE){
        drawSelectNum();
        drawSound();
    }
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

HCombGLWidget::HCombGLWidget(QWidget* parent)
    : HGLWidget(parent)
{
    m_bMouseMoving = false;
    m_combtype = UNKNOW;

    initUI();
    initConnect();
}

HCombGLWidget::~HCombGLWidget(){

}

void HCombGLWidget::initUI(){
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(2);

    m_titlebar = new HCombTitlebarWidget;
    m_titlebar->setFixedHeight(TITLE_BAR_HEIGHT);
    m_titlebar->hide();
    vbox->addWidget(m_titlebar);

    vbox->addStretch();

    m_toolbar = new HCombToolbarWidget;
    m_toolbar->setFixedHeight(TOOL_BAR_HEIGHT);
    m_toolbar->hide();
    vbox->addWidget(m_toolbar);

    setLayout(vbox);

    m_labelDrag = new QLabel(this);
    m_labelDrag->setStyleSheet("border:3px groove #FF8C00");
    m_labelDrag->hide();

    m_labelAdd = new QLabel(this);
    m_labelAdd->setStyleSheet("border:3px dashed white");
    m_labelAdd->hide();

    m_wdgTrash = new HChangeColorWidget(this);
    m_wdgTrash->setPixmap(HRcLoader::instance()->icon_trash_big);
    m_wdgTrash->hide();

    m_wdgExpre = new HExpreWidget(this);
    m_wdgExpre->setWindowFlags(Qt::Popup);
}

void HCombGLWidget::initConnect(){
    QObject::connect( g_dsCtx, SIGNAL(combChanged()), this, SLOT(onCombChanged()) );

    QObject::connect( m_titlebar->m_btnFullScreen, SIGNAL(clicked(bool)), this, SIGNAL(fullScreen()) );
    QObject::connect( m_titlebar->m_btnExitFullScreen, SIGNAL(clicked(bool)), this, SIGNAL(exitFullScreen()) );
    QObject::connect( m_titlebar->m_btnSnapshot, SIGNAL(clicked(bool)), this, SLOT(snapshot()) );
    //QObject::connect( m_titlebar->m_btnStartRecord, SIGNAL(clicked(bool)), this, SLOT(startRecord()) );
    //QObject::connect( m_titlebar->m_btnStopRecord, SIGNAL(clicked(bool)), this, SLOT(stopRecord()) );

    QObject::connect( m_toolbar->m_btnStart, SIGNAL(clicked(bool)), this, SLOT(onStart()) );
    QObject::connect( m_toolbar->m_btnPause, SIGNAL(clicked(bool)), this, SLOT(onPause()) );
    QObject::connect( m_toolbar->m_btnUndo, SIGNAL(clicked(bool)), this, SIGNAL(undo()) );
    QObject::connect( m_toolbar->m_btnTrash, SIGNAL(clicked(bool)), this, SLOT(onTrash()) );
    QObject::connect( m_toolbar->m_btnExpre, SIGNAL(clicked(bool)), this, SLOT(showExpre()) );
    QObject::connect( m_toolbar->m_btnOK, SIGNAL(clicked(bool)), this, SLOT(onOK()) );

    QObject::connect( m_wdgExpre, SIGNAL(expreSelected(QString&)), this, SLOT(onExpreSelected(QString&)) );
}

void HCombGLWidget::showTitlebar(bool bShow){
    if (bShow){
        DsSvrItem* item = g_dsCtx->getItem(svrid);
        if (item){
            m_titlebar->m_label->setText(item->title.c_str());
        }
        m_titlebar->show();
    }else{
        m_titlebar->hide();
    }
}

void HCombGLWidget::showToolbar(bool bShow){
    if (bShow){
        if ((m_status & MAJOR_STATUS_MASK) == PLAYING){
            m_toolbar->m_btnStart->hide();
            m_toolbar->m_btnPause->show();
        }
        if ((m_status & MAJOR_STATUS_MASK) == PAUSE){
            m_toolbar->m_btnStart->show();
            m_toolbar->m_btnPause->hide();
        }
        m_toolbar->show();
    }else{
        m_toolbar->hide();
    }
}

void HCombGLWidget::showToolWidgets(bool bShow){
    HGLWidget::showToolWidgets(bShow);

    showTitlebar(bShow);
    showToolbar(bShow);
    m_wdgTrash->setVisible(bShow);
}

void HCombGLWidget::onTargetChanged(){
    if (m_labelAdd->isVisible()){
        if (m_target.type == LABEL_ADD)
            m_labelAdd->setStyleSheet("border:3px dashed red");
        else
            m_labelAdd->setStyleSheet("border:3px dashed white");
    }
}

#define LOCATION_PADDING    32
int HCombGLWidget::getLocation(QPoint pt, QRect rc){
    int loc = NotIn;
    if (rc.contains(pt)){
         if (pt.x() - rc.left() < LOCATION_PADDING)
             loc |= Left;
         if (rc.right() - pt.x() < LOCATION_PADDING)
             loc |= Right;
         if (pt.y() - rc.top() < LOCATION_PADDING)
             loc |= Top;
         if (rc.bottom() - pt.y() < LOCATION_PADDING)
             loc |= Bottom;
         return loc == NotIn ? Center : loc;
    }

    return NotIn;
}

HCombGLWidget::TargetInfo HCombGLWidget::getTargetByPos(QPoint pt, TRAGET_TYPE type){
    TargetInfo ti;

    // 优先顺序: STOPWATCH > TIME > TEXT > PICTURE > SCREEN

    if (type == ALL){
        if (m_labelAdd->isVisible() && m_labelAdd->geometry().contains(pt)){
            ti.type = LABEL_ADD;
            ti.index = 0;
            ti.location = getLocation(pt, m_labelAdd->geometry());
            return ti;
        }
    }

    if (type == ALL || type == STOPWATCH){

    }

    if (type == ALL || type == TIME){

    }

    if (type == ALL || type == TEXT){

    }

    if (type == ALL || type == PICTURE){

    }

    if (type == ALL || type == SCREEN){
            // sub screen > main screen
        for (int i = m_vecScreens.size()-1; i >= 0; --i){
            if (m_vecScreens[i].contains(pt)){
                ti.type = SCREEN;
                ti.index = i;
                ti.location = getLocation(pt, m_vecScreens[i]);
                return ti;
            }
        }
    }

    return ti;
}

void HCombGLWidget::adjustPos(QRect &rc){
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

void HCombGLWidget::onCombChanged(){
    // scale
    double scale_x = (double)width() / (double)g_dsCtx->m_tComb.width;
    double scale_y = (double)height() / (double)g_dsCtx->m_tComb.height;
    m_vecScreens.clear();
    for (int i = 0; i < g_dsCtx->m_tComb.itemCnt; ++i){
        int x = g_dsCtx->m_tComb.items[i].x * scale_x + 0.5;
        int y = g_dsCtx->m_tComb.items[i].y * scale_y + 0.5;
        int w = g_dsCtx->m_tComb.items[i].w * scale_x + 0.5;
        int h = g_dsCtx->m_tComb.items[i].h * scale_y + 0.5;
        QRect rc(x,y,w,h);
        m_vecScreens.push_back(rc);
    }

    // add comb_type for PIP not move main screen
    if (m_vecScreens.size() > 0){
        if (m_vecScreens[0].width() > width()-10 && m_vecScreens[0].height() > height()-10){
            m_combtype = PIP;
        }else{
            m_combtype = TILED;
        }
    }
}

void HCombGLWidget::onTrash(){
    if (m_target.type == LABEL_ADD){
        m_labelAdd->hide();
    }else if (m_target.type == SCREEN){
        stopComb(m_target.index);
    }
}

void HCombGLWidget::onOK(){
    if (m_labelAdd->isVisible()){
        m_labelAdd->hide();
        //...
    }
}

void HCombGLWidget::showExpre(){
    int w = EXPRE_ICON_WIDTH * 4 + 57;
    int h = EXPRE_ICON_HEIGHT * 3 + 22;
    m_wdgExpre->setGeometry(x() + (width() - w)/2, m_toolbar->y() + y() - h, w, h);
    m_wdgExpre->show();
}

#define EXPRE_MAX_WIDTH     360
#define EXPRE_MAX_HEIGHT    360
void HCombGLWidget::onExpreSelected(QString& filepath){
    m_pixmapAdd.load(filepath);
    if (m_pixmapAdd.isNull())
        return;

    QPixmap pixmap = m_pixmapAdd;
    int w = pixmap.width();
    int h = pixmap.height();
    if (w > EXPRE_MAX_WIDTH || h > EXPRE_MAX_HEIGHT){
        w = EXPRE_MAX_WIDTH;
        h = EXPRE_MAX_HEIGHT;
        pixmap = pixmap.scaled(QSize(w,h));
    }

    m_labelAdd->setGeometry((width()-w)/2, (height()-h)/2, w, h);
    m_labelAdd->setPixmap(pixmap);
    m_labelAdd->show();

    m_target.type = LABEL_ADD;
    m_target.index = 0;
    onTargetChanged();
}

void HCombGLWidget::drawOutline(){
    DrawInfo di;
    di.left = 0;
    di.top = 0;
    di.right = width() - 1;
    di.bottom = height() - 1;
    di.color = m_outlinecolor;
    drawRect(&di, 3);
}

void HCombGLWidget::drawTaskInfo(){
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

void HCombGLWidget::drawCombInfo(){
    DrawInfo di;
    for (int i = 0; i < m_vecScreens.size(); ++i){
        // draw comb NO.
        di.left = m_vecScreens[i].left() + 1;
        di.bottom = m_vecScreens[i].bottom() - 1;
        di.top = di.bottom - 48 + 1;
        di.right = di.left + 48 - 1;
        drawTex(&HRcLoader::instance()->tex_numr[i], &di);

        // draw comb outline
        di.left = m_vecScreens[i].left();
        di.top = m_vecScreens[i].top();
        di.right = m_vecScreens[i].right();
        di.bottom = m_vecScreens[i].bottom();
        di.color = m_outlinecolor;
        drawRect(&di);
    }
}

void HCombGLWidget::paintGL(){
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
        drawCombInfo();
    }

    // draw focused target outline
    if (m_titlebar->isVisible()){
        DrawInfo di;
        if (m_target.type == SCREEN){
            di.left = m_vecScreens[m_target.index].left();
            di.top = m_vecScreens[m_target.index].top();
            di.right = m_vecScreens[m_target.index].right();
            di.bottom = m_vecScreens[m_target.index].bottom();
            di.color = g_dsCtx->m_tInit.focus_outlinecolor;
        }
        drawRect(&di, 3);
    }
}

void HCombGLWidget::resizeEvent(QResizeEvent *e){
    m_wdgTrash->setGeometry(width()-128-1, height()/2-64, 128, 128);

    onCombChanged();

    HGLWidget::resizeEvent(e);
}

void HCombGLWidget::mousePressEvent(QMouseEvent* e){
    HGLWidget::mousePressEvent(e);

    m_target = getTargetByPos(e->pos());
    onTargetChanged();
}

void HCombGLWidget::mouseMoveEvent(QMouseEvent *e){
    HGLWidget::mouseMoveEvent(e);
    if (e->isAccepted())
        return;

    if (!rect().contains(e->pos())){
        e->accept();
        return;
    }

    if (!m_bMouseMoving){
        m_bMouseMoving = true;
        // moveBegin
        if (m_target.type == LABEL_ADD){
            return;
        }else if (m_target.type == SCREEN){
            if (!(status(MAJOR_STATUS_MASK) == PLAYING || status(MAJOR_STATUS_MASK) == PAUSE))
                return;

            QRect rc = m_vecScreens[m_target.index];
            m_pixmapDrag = grab(rc);
            m_labelDrag->setGeometry(rc);
            m_labelDrag->setPixmap(m_pixmapDrag);
            m_labelDrag->show();

            if (m_target.index == 0){
                if (m_wdgTrash->isVisible()){
                    m_labelDrag->setGeometry(0,0,DRAG_WIDTH,DRAG_HEIGHT);
                    m_labelDrag->setPixmap(m_pixmapDrag.scaled(DRAG_WIDTH,DRAG_HEIGHT));
                }else{
                    m_labelDrag->hide();
                }
            }
        }
    }

    if (m_labelDrag->isVisible()){
        QRect rc = m_labelDrag->geometry();

        if (m_target.location & Center){
            // move
            int w = m_labelDrag->width();
            int h = m_labelDrag->height();
            rc.setRect(e->x()-w/2, e->y()-h/2, w, h);
            adjustPos(rc);
        }else{
            // resize
            if (m_target.location & Left){
                if (e->x() < rc.right() - 2*LOCATION_PADDING)
                    rc.setLeft(e->x());
            }else if (m_target.location & Right){
                if (e->x() > rc.left() + 2*LOCATION_PADDING)
                rc.setRight(e->x());
            }
            if (m_target.location & Top){
                if (e->y() < rc.bottom() - 2*LOCATION_PADDING)
                    rc.setTop(e->y());
            }else if (m_target.location & Bottom){
                if (e->y() > rc.top() + 2*LOCATION_PADDING)
                    rc.setBottom(e->y());
            }
            m_labelDrag->setPixmap(m_pixmapDrag.scaled(rc.size()));
        }

        m_labelDrag->setGeometry(rc);
    }

    if (m_target.type == LABEL_ADD){
        if (m_labelAdd->isVisible()){
            QRect rc = m_labelAdd->geometry();

            if (m_target.location & Center){
                // move
                int w = m_labelAdd->width();
                int h = m_labelAdd->height();
                rc.setRect(e->x()-w/2, e->y()-h/2, w, h);
                adjustPos(rc);
            }else{
                // resize
                if (m_target.location & Left){
                    if (e->x() < rc.right() - 2*LOCATION_PADDING)
                        rc.setLeft(e->x());
                }else if (m_target.location & Right){
                    if (e->x() > rc.left() + 2*LOCATION_PADDING)
                    rc.setRight(e->x());
                }
                if (m_target.location & Top){
                    if (e->y() < rc.bottom() - 2*LOCATION_PADDING)
                        rc.setTop(e->y());
                }else if (m_target.location & Bottom){
                    if (e->y() > rc.top() + 2*LOCATION_PADDING)
                        rc.setBottom(e->y());
                }
                m_labelAdd->setPixmap(m_pixmapAdd.scaled(rc.size()));
            }

            m_labelAdd->setGeometry(rc);
        }
    }

    if (m_wdgTrash->isVisible()){
        if (m_wdgTrash->geometry().contains(e->pos())){
            m_wdgTrash->changeColor(QColor(255, 0, 0, 128));
        }else{
            m_wdgTrash->changeColor(Qt::transparent);
        }
    }
}

void HCombGLWidget::mouseReleaseEvent(QMouseEvent *e){
    HGLWidget::mouseReleaseEvent(e);

    if (m_bMouseMoving){
        m_bMouseMoving = false;
        // moveEnd

        m_labelDrag->hide();

        if (m_wdgTrash->isVisible() && m_wdgTrash->geometry().contains(e->pos())){
            onTrash();
        }

        if (m_target.type == SCREEN){
            if (m_target.index == 0 && m_combtype == PIP)
                return; // main screen not move or resize
            reposComb(m_target.index, m_labelDrag->geometry());
        }
    }
}

void HCombGLWidget::reposComb(int index, QRect rc){
    if (rc == m_vecScreens[index])
        return;

    DsCombInfo ci = g_dsCtx->m_tComb;

    double scale_x = (double)g_dsCtx->m_tComb.width / (double)width();
    double scale_y = (double)g_dsCtx->m_tComb.height / (double)height();
    ci.items[index].x = rc.x() * scale_x;
    ci.items[index].y = rc.y() * scale_y;
    ci.items[index].w = rc.width() * scale_x;
    ci.items[index].h = rc.height() * scale_y;

    emit combChanged(ci);
}

void HCombGLWidget::stopComb(int index){
//    DsEvent evt;
//    evt.type = DS_EVENT_STOP;
//    evt.dst_svrid = 1;
//    evt.dst_x = m_ptMousePressed.x();
//    evt.dst_y = m_ptMousePressed.y();
//    g_dsCtx->handle_event(evt);

    DsCombInfo ci = g_dsCtx->m_tComb;
    if (ci.items[index].iSvrid != 0){
        ci.items[index].iSvrid = 0;
        ci.items[index].bAudio = false;
        emit combChanged(ci);
    }
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
