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
    m_bMousePressed = false;
    m_tmMousePressed = 0;

    initUI();
    initConnect();
}

HGLWidget::~HGLWidget(){

}

void HGLWidget::initUI(){
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(2);

    m_titleWdg = new HTitlebarWidget;
    m_titleWdg->setFixedHeight(TITLE_BAR_HEIGHT);
    m_titleWdg->hide();
    vbox->addWidget(m_titleWdg);

    vbox->addStretch();

    m_toolWdg = new HToolbarWidget;
    m_toolWdg->setFixedHeight(TOOL_BAR_HEIGHT);
    m_toolWdg->hide();
    vbox->addWidget(m_toolWdg);

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
    QObject::connect( m_titleWdg, SIGNAL(fullScreen()), this, SIGNAL(fullScreen()) );
    QObject::connect( m_titleWdg, SIGNAL(exitFullScreen()), this, SIGNAL(exitFullScreen()) );
    QObject::connect( m_titleWdg->m_btnSnapshot, SIGNAL(clicked(bool)), this, SLOT(snapshot()) );
    //QObject::connect( m_titleWdg->m_btnStartRecord, SIGNAL(clicked(bool)), this, SLOT(startRecord()) );
    //QObject::connect( m_titleWdg->m_btnStopRecord, SIGNAL(clicked(bool)), this, SLOT(stopRecord()) );
    QObject::connect( m_titleWdg->m_btnNum, SIGNAL(clicked(bool)), this, SLOT(showNumSelector()) );

    QObject::connect( m_toolWdg, SIGNAL(sigStart()), this, SLOT(onStart()) );
    QObject::connect( m_toolWdg, SIGNAL(sigPause()), this, SLOT(onPause()) );
    QObject::connect( m_toolWdg, SIGNAL(sigStop()), this, SLOT(onStop()) );
    QObject::connect( m_toolWdg, SIGNAL(progressChanged(int)), this, SLOT(onProgressChanged(int)) );

    QObject::connect( m_numSelector, SIGNAL(numSelected(int)), this, SLOT(onNumSelected(int)) );
    QObject::connect( m_numSelector, SIGNAL(numCanceled(int)), this, SLOT(onNumCanceled(int)) );
}

void HGLWidget::showTitlebar(bool bShow){
    if (bShow){
        if ((m_status & MAJOR_STATUS_MASK) == PLAYING ||
            (m_status & MAJOR_STATUS_MASK) == PAUSE){
            DsItemInfo* item = g_dsCtx->getItem(svrid);
            if (item){
                m_titleWdg->setTitle(item->title.c_str());
            }
            m_titleWdg->show();
            setOutlineColor(g_dsCtx->focus_outlinecolor);
        }
    }else{
        m_titleWdg->hide();
        setOutlineColor(g_dsCtx->outlinecolor);
    }
}

void HGLWidget::showToolbar(bool bShow){
    if (bShow){
        if ((m_status & MAJOR_STATUS_MASK) == PLAYING ||
            (m_status & MAJOR_STATUS_MASK) == PAUSE){

            if ((m_status & MAJOR_STATUS_MASK) == PLAYING){
                m_toolWdg->m_btnStart->hide();
                m_toolWdg->m_btnPause->show();
            }
            if ((m_status & MAJOR_STATUS_MASK) == PAUSE){
                m_toolWdg->m_btnStart->show();
                m_toolWdg->m_btnPause->hide();
            }

            if (g_dsCtx->getItem(svrid)->src_type == SRC_TYPE_FILE){
                m_toolWdg->m_slider->show();
                m_toolWdg->show();
            }
            if (svrid == 1){
                m_toolWdg->m_slider->hide();
                m_toolWdg->show();
            }
        }
    }else{
        m_toolWdg->hide();
    }
}

void HGLWidget::toggleTitlebar(){
    if (m_titleWdg->isVisible()){
        showTitlebar(false);
    }else{
        showTitlebar(true);
    }
}

void HGLWidget::toggleToolbar(){
    if (m_toolWdg->isVisible()){
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
    DsItemInfo* item = g_dsCtx->getItem(svrid);
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
    DsItemInfo* item = g_dsCtx->getItem(svrid);
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
    DsItemInfo* item = g_dsCtx->getItem(svrid);
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
    DsItemInfo* item = g_dsCtx->getItem(svrid);
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
    g_dsCtx->m_tOriginCocks[num - 1].iSvrid = svrid;
}

void HGLWidget::onNumCanceled(int num){
    g_dsCtx->m_tOriginCocks[num - 1].iSvrid = 0;
}

void HGLWidget::showNumSelector(){
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        if (g_dsCtx->m_tOriginCocks[i].iSvrid == svrid){
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
    m_bMousePressed = true;
    m_tmMousePressed = event->timestamp();
    event->ignore();
}

void HGLWidget::mouseReleaseEvent(QMouseEvent* event){
    QRect rc(0, 0, width(), height());
    if (m_bMousePressed && (event->timestamp() - m_tmMousePressed < 200) &&
            rc.contains(event->x(), event->y())){
        toggleToolWidgets();
        emit clicked();
    }

    m_bMousePressed = false;
    event->ignore();
}

void HGLWidget::mouseMoveEvent(QMouseEvent* e){
    // add delay to prevent misopration
    if (m_bMousePressed && (e->timestamp() - m_tmMousePressed < 200)){
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

void HGLWidget::paintGL(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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
        DsItemInfo* item = g_dsCtx->getItem(svrid);
        if (!item)
            break;
        if (m_status & PLAY_VIDEO){
            // draw yuv
            item->mutex.lock();
            if (item->tex_yuv.data && item->tex_yuv.width > 0 && item->tex_yuv.height > 0){
                drawYUV(&item->tex_yuv);
                m_nPreFrame = item->v_input;
            }
            item->mutex.unlock();
        }

        if (m_status & PLAY_AUDIO){
            if (m_bDrawAudio){
                // draw sound average
                if (item->a_channels > 1){
                    di.left = width()-4 - AUDIO_WIDTH*2;
                    di.top = height()-2 - AUDIO_HEIGHT;
                    di.right = di.left + AUDIO_WIDTH;
                    di.bottom = di.top + AUDIO_HEIGHT;
                    di.color = 0x00FFFF80;
                    drawRect(&di, true);

                    di.left += 1;
                    di.right -= 1;
                    di.bottom -= 1;
                    di.top = di.bottom - item->a_average[1] * AUDIO_HEIGHT / 65536;
                    di.color = 0xFFFF0080;
                    drawRect(&di, true);
                }

                di.left = width()-2 - AUDIO_WIDTH;
                di.top = height()-2 - AUDIO_HEIGHT;
                di.right = di.left + AUDIO_WIDTH;
                di.bottom = di.top + AUDIO_HEIGHT;
                di.color = 0x00FFFF80;
                drawRect(&di, true);

                di.left += 1;
                di.right -= 1;
                di.bottom -= 1;
                di.top = di.bottom - item->a_average[0] * AUDIO_HEIGHT / 65536;
                di.color = 0xFFFF0080;
                drawRect(&di, true);

                item->bUpdateAverage = true;
            }
        }

        // draw select num
        di.top = height()-48;
        di.bottom = height()-1;
        di.left = 1;
        di.right = 48;
        for (int i = 0; i < MAX_NUM_ICON; ++i){
            if (g_dsCtx->m_tOriginCocks[i].iSvrid == svrid){
                drawTex(&HRcLoader::instance()->tex_numr[i], &di);
                di.left += 48;
                di.right += 48;
                if (g_dsCtx->m_tOriginCocks[i].bAudio){
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
        break;
    }

    // draw title
    if (m_bDrawTitle){
        DsItemInfo* item = g_dsCtx->getItem(svrid);
        if (item && item->title.length() > 0){
            di.left = 2;
            di.top = 2;
            di.color = m_titcolor;
            drawStr(g_dsCtx->m_pFont, item->title.c_str(), &di);
        }
    }

    // draw icons
    m_mutex.lock();
    std::map<int,DrawInfo>::iterator iter = m_mapIcons.begin();
    while (iter != m_mapIcons.end()){
        Texture *tex = getTexture(iter->first);
        di = iter->second;
        if (tex){
            drawTex(tex, &di);
        }
        ++iter;
    }
    m_mutex.unlock();

    // draw outline
    di.left = 0;
    di.top = 0;
    di.right = width() - 1;
    di.bottom = height() - 1;
    di.color = m_outlinecolor;
    drawRect(&di);
}

//===============================================================================

HCockGLWidget::HCockGLWidget(QWidget* parent)
    : HGLWidget(parent)
{
    m_cockoutlinecolor = 0xFFFFFFFF;

    m_titleWdg->m_btnNum->hide();

    m_labelDrag = new QLabel(this);
    m_labelDrag->setStyleSheet("border:3px groove #FF8C00");
    m_labelDrag->hide();

    m_wdgTrash = new HChangeColorWidget(this);
    m_wdgTrash->setPixmap(HRcLoader::instance()->icon_trash);
    m_wdgTrash->hide();

    QObject::connect( g_dsCtx, SIGNAL(cockChanged()), this, SLOT(onCockChanged()) );
}

HCockGLWidget::~HCockGLWidget(){

}

void HCockGLWidget::toggleToolWidgets(){
    HGLWidget::toggleToolWidgets();
    toggleTrash();
}

int HCockGLWidget::getCockByPos(QPoint pt, QRect& rc){
    for (int i = 1; i < m_vecCocks.size(); ++i){
        if (m_vecCocks[i].contains(pt)){
            rc = m_vecCocks[i];
            return i;
        }
    }

    rc = m_vecCocks[0];
    return 0;
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
    double scale_x = (double)width() / (double)g_dsCtx->m_iOriginCockW;
    double scale_y = (double)height() / (double)g_dsCtx->m_iOriginCockH;
    m_vecCocks.clear();
    for (int i = 0; i < g_dsCtx->m_cntCock; ++i){
        int x = g_dsCtx->m_tOriginCocks[i].x * scale_x + 0.5;
        int y = g_dsCtx->m_tOriginCocks[i].y * scale_y + 0.5;
        int w = g_dsCtx->m_tOriginCocks[i].w * scale_x + 0.5;
        int h = g_dsCtx->m_tOriginCocks[i].h * scale_y + 0.5;
        QRect rc(x,y,w,h);
        m_vecCocks.push_back(rc);
    }
}

void HCockGLWidget::paintGL(){
    if (g_dsCtx->info){
        m_bDrawTitle = true;
        m_bDrawAudio = true;
    }else{
        m_bDrawTitle = false;
        m_bDrawAudio = false;
    }
    HGLWidget::paintGL();

    if (g_dsCtx->info){
        DrawInfo di;
        // draw taskinfo
        if (g_dsCtx->m_pFont){
            int oldSize = g_dsCtx->m_pFont->FaceSize();
            g_dsCtx->m_pFont->FaceSize(32);
            separator sept(g_dsCtx->m_strTaskInfo.c_str(), "\r\n");
            di.top = 10;
            di.left = 10;
            di.color = g_dsCtx->infcolor;
            for (int i = 0; i < sept.size(); ++i){
                drawStr(g_dsCtx->m_pFont, sept[i], &di);
                di.top += g_dsCtx->m_pFont->LineHeight() + 10;
            }
            g_dsCtx->m_pFont->FaceSize(oldSize);
        }

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
}

void HCockGLWidget::resizeEvent(QResizeEvent *e){
    onCockChanged();
    m_wdgTrash->setGeometry(width()-128-1, height()/2-64, 128, 128);

    HGLWidget::resizeEvent(e);
}

void HCockGLWidget::mouseMoveEvent(QMouseEvent *e){
    HGLWidget::mouseMoveEvent(e);
    if (e->isAccepted())
        return;

    if (status(MAJOR_STATUS_MASK) == PLAYING && !m_labelDrag->isVisible()){
        m_ptDrag.setX(e->x());
        m_ptDrag.setY(e->y());

        QRect rc;
        m_indexDrag = getCockByPos(m_ptDrag, rc);
        if (m_wdgTrash->isVisible()){
            m_labelDrag->setFixedSize(DRAG_WIDTH, DRAG_HEIGHT);
            m_labelDrag->setPixmap(grab(rc).scaled(DRAG_WIDTH, DRAG_HEIGHT));
        }else{
            m_labelDrag->setFixedSize(rc.size());
            m_labelDrag->setPixmap(grab(rc));
        }
        m_labelDrag->show();
    }

    if (m_labelDrag->isVisible()){
        int w = m_labelDrag->width();
        int h = m_labelDrag->height();
        QRect rc(e->x()-w/2, e->y()-w/2, w, h);

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
}

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
void HCockGLWidget::mouseReleaseEvent(QMouseEvent *e){
    HGLWidget::mouseReleaseEvent(e);

    if (m_labelDrag->isVisible()){
        m_labelDrag->hide();

        if (m_wdgTrash->isVisible()){
            if (m_wdgTrash->geometry().contains(e->pos())){
                // stop cock
                DsEvent evt;
                evt.type = DS_EVENT_STOP;
                evt.dst_svrid = 1;
                evt.dst_x = m_ptDrag.x();
                evt.dst_y = m_ptDrag.y();
                g_dsCtx->handle_event(evt);
            }
        }else{
            // move cock pos
            QRect rcDst = m_labelDrag->geometry();
            if (rcDst == m_vecCocks[m_indexDrag]){
                qDebug("");
                return;
            }
            double scale_x = (double)g_dsCtx->m_iOriginCockW / (double)width();
            double scale_y = (double)g_dsCtx->m_iOriginCockH / (double)height();
            int x = rcDst.x() * scale_x;
            int y = rcDst.y() * scale_y;
            int w = rcDst.width() * scale_x;
            int h = rcDst.height() * scale_y;

            QJsonArray arr;
            for (int i = 0; i < g_dsCtx->m_cntCock; ++i){
                QJsonObject obj;
                DsCockInfo* info = &g_dsCtx->m_tOriginCocks[i];
                if (i == m_indexDrag){
                    obj.insert("x", x);
                    obj.insert("y", y);
                    obj.insert("w", w);
                    obj.insert("h", h);
                }else{
                    obj.insert("x", info->x);
                    obj.insert("y", info->y);
                    obj.insert("w", info->w);
                    obj.insert("h", info->h);
                }
                obj.insert("v", info->iSvrid);
                obj.insert("a", info->bAudio ? 1 : 0);
                arr.append(obj);
            }
            QJsonDocument doc;
            doc.setArray(arr);
            QByteArray bytes = doc.toJson();
            qDebug(bytes.constData());
            emit cockRepos(bytes);
        }
    }
}
