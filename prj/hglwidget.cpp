#include "ds_global.h"
#include "hglwidget.h"
#include "hdsctx.h"
#include "hrcloader.h"
#include "ds.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
bool QGLWidgetImpl::s_bInitGLEW = false;
GLuint QGLWidgetImpl::prog_yuv;
GLuint QGLWidgetImpl::texUniformY;
GLuint QGLWidgetImpl::texUniformU;
GLuint QGLWidgetImpl::texUniformV;

QGLWidgetImpl::QGLWidgetImpl(QWidget* parent)
    : QOpenGLWidget(parent)
{

}

QGLWidgetImpl::~QGLWidgetImpl()
{

}

void QGLWidgetImpl::initializeGL(){
    if (!s_bInitGLEW){
        if (glewInit() != 0){
            qFatal("glewInit failed");
            return;
        }
        loadYUVShader();
        s_bInitGLEW = true;
    }

    initVAO();
}

void QGLWidgetImpl::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
}

void QGLWidgetImpl::loadYUVShader(){
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    char szVS[] = "				\
    attribute vec4 verIn;		\
    attribute vec2 texIn;		\
    varying vec2 texOut;		\
                                \
    void main(){				\
        gl_Position = verIn;	\
        texOut = texIn;			\
    }							\
    ";
    const GLchar* pszVS = szVS;
    GLint len = strlen(szVS);
    glShaderSource(vs, 1, (const GLchar**)&pszVS, &len);

    char szFS[] = "				\
    varying vec2 texOut;		\
    uniform sampler2D tex_y;	\
    uniform sampler2D tex_u;	\
    uniform sampler2D tex_v;	\
                                \
    void main(){				\
        vec3 yuv;				\
        vec3 rgb;				\
        yuv.x = texture2D(tex_y, texOut).r;			\
        yuv.y = texture2D(tex_u, texOut).r - 0.5;	\
        yuv.z = texture2D(tex_v, texOut).r - 0.5;	\
        rgb = mat3( 1,       1,         1,			\
            0,       -0.39465,  2.03211,			\
            1.13983, -0.58060,  0) * yuv;			\
        gl_FragColor = vec4(rgb, 1);				\
    }												\
    ";
    const GLchar* pszFS = szFS;
    len = strlen(szFS);
    glShaderSource(fs, 1, (const GLchar**)&pszFS, &len);

    glCompileShader(vs);
    glCompileShader(fs);

//#ifdef _DEBUG
    GLint iRet = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &iRet);
    qDebug("vs::GL_COMPILE_STATUS=%d", iRet);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &iRet);
    qDebug("fs::GL_COMPILE_STATUS=%d", iRet);
//#endif

    prog_yuv = glCreateProgram();

    glAttachShader(prog_yuv, vs);
    glAttachShader(prog_yuv, fs);

    glBindAttribLocation(prog_yuv, ver_attr_ver, "verIn");
    glBindAttribLocation(prog_yuv, ver_attr_tex, "texIn");

    glLinkProgram(prog_yuv);

//#ifdef _DEBUG
    glGetProgramiv(prog_yuv, GL_LINK_STATUS, &iRet);
    qDebug("prog_yuv=%d GL_LINK_STATUS=%d", prog_yuv, iRet);
//#endif

    glValidateProgram(prog_yuv);

    texUniformY = glGetUniformLocation(prog_yuv, "tex_y");
    texUniformU = glGetUniformLocation(prog_yuv, "tex_u");
    texUniformV = glGetUniformLocation(prog_yuv, "tex_v");

    qDebug("loadYUVShader ok");

    //glUseProgram(prog_yuv);
}

void QGLWidgetImpl::initVAO(){
    static const GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    static const GLfloat textures[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    // reverse
    //static const GLfloat textures[] = {
    //	0.0f, 0.0f,
    //	1.0f, 0.0f,
    //	0.0f, 1.0f,
    //	1.0f, 1.0f,
    //};

    //glGenBuffers(buffer_num, g_buffers);
    //glBindBuffer(GL_ARRAY_BUFFER, g_buffers[buffer_ver]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glVertexAttribPointer(ver_attr_ver, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(ver_attr_ver, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(ver_attr_ver);

    //glGenBuffers(buffer_num, g_buffers);
    //glBindBuffer(GL_ARRAY_BUFFER, g_buffers[buffer_tex]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
    //glVertexAttribPointer(ver_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(ver_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, textures);
    glEnableVertexAttribArray(ver_attr_tex);

    glGenTextures(3, tex_yuv);
    for (int i = 0; i < 3; i++){
        glBindTexture(GL_TEXTURE_2D, tex_yuv[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void QGLWidgetImpl::drawYUV(Texture* tex){
    glUseProgram(prog_yuv);

    int w = tex->width;
    int h = tex->height;
    //qDebug("drawYUV w=%d h=%d", w, h);
    int y_size = w*h;
    GLubyte* y = tex->data;
    GLubyte* u = y + y_size;
    GLubyte* v = u + (y_size>>2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_yuv[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, y);
    glUniform1i(texUniformY, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_yuv[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w/2, h/2, 0, GL_RED, GL_UNSIGNED_BYTE, u);
    glUniform1i(texUniformU, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_yuv[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w/2, h/2, 0, GL_RED, GL_UNSIGNED_BYTE, v);
    glUniform1i(texUniformV, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
}

void QGLWidgetImpl::drawTex(Texture* tex, DrawInfo* di){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->texID);
    gluBuild2DMipmaps(GL_TEXTURE_2D, tex->bpp/8, tex->width, tex->height, tex->type, GL_UNSIGNED_BYTE, tex->data);
    //glTexImage2D(GL_TEXTURE_2D, 0, tex->bpp/8, tex->width, tex->height, 0, tex->type, GL_UNSIGNED_BYTE, tex->data);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    //glTexCoord2d(0,0);glVertex2f(-1, -1);
    //glTexCoord2d(1,0);glVertex2f( 1, -1);
    //glTexCoord2d(1,1);glVertex2f( 1,  1);
    //glTexCoord2d(0,1);glVertex2f(-1,  1);
    glTexCoord2d(0,0);glVertex2i(di->left, di->bottom);
    glTexCoord2d(1,0);glVertex2i(di->right, di->bottom);
    glTexCoord2d(1,1);glVertex2f(di->right, di->top);
    glTexCoord2d(0,1);glVertex2f(di->left, di->top);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void QGLWidgetImpl::drawStr(FTGLPixmapFont *pFont, const char* str, DrawInfo* di){
    if (!pFont)
        return ;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

//    const char* mbs = str;
//    int len_mbs = strlen(mbs);
//#ifdef WIN32
//    setlocale(LC_ALL,".936");
//#else
//    setlocale(LC_ALL,"zh_CN.utf8");
//#endif
//    int len_wcs = mbstowcs(NULL, mbs, 0);
//    wchar_t* wcs = new wchar_t[len_wcs + 1];
//    mbstowcs(wcs, mbs, strlen(mbs)+1);

    glColor3ub(R(di->color), G(di->color), B(di->color));
    glRasterPos2i(di->left, di->top + pFont->LineHeight());
    pFont->Render(str);
    glColor3ub(255,255,255);

    //delete[] wcs;
}

void QGLWidgetImpl::drawRect(DrawInfo* di, bool bFill){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    if (bFill){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4ub(R(di->color), G(di->color), B(di->color), A(di->color));
    glRecti(di->left, di->top, di->right, di->bottom);
    glColor4ub(255,255,255,255);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

HGLWidget::HGLWidget(QWidget *parent)
    : QGLWidgetImpl(parent)
{
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
}

void HGLWidget::initConnect(){
    QObject::connect( m_titleWdg, SIGNAL(fullScreen()), this, SIGNAL(fullScreen()) );
    QObject::connect( m_titleWdg, SIGNAL(exitFullScreen()), this, SIGNAL(exitFullScreen()) );
    QObject::connect( m_titleWdg->m_btnSnapshot, SIGNAL(clicked(bool)), this, SLOT(snapshot()) );
    //QObject::connect( m_titleWdg->m_btnStartRecord, SIGNAL(clicked(bool)), this, SLOT(startRecord()) );
    //QObject::connect( m_titleWdg->m_btnStopRecord, SIGNAL(clicked(bool)), this, SLOT(stopRecord()) );
    QObject::connect( m_titleWdg, SIGNAL(numSelected(int)), this, SLOT(onNumSelected(int)) );
    QObject::connect( m_titleWdg, SIGNAL(numUnselected(int)), this, SLOT(onNumUnselected(int)) );

    QObject::connect( m_toolWdg, SIGNAL(sigStart()), this, SLOT(onStart()) );
    QObject::connect( m_toolWdg, SIGNAL(sigPause()), this, SLOT(onPause()) );
    QObject::connect( m_toolWdg, SIGNAL(sigStop()), this, SLOT(onStop()) );
    QObject::connect( m_toolWdg, SIGNAL(progressChanged(int)), this, SLOT(onProgressChanged(int)) );
}

void HGLWidget::showTitlebar(bool bShow){
    if (bShow){
        if ((m_status & MAJOR_STATUS_MASK) == PLAYING ||
            (m_status & MAJOR_STATUS_MASK) == PAUSE){
            DsItemInfo* item = g_dsCtx->getItem(svrid);
            if (item){
                m_titleWdg->setTitle(item->title.c_str());
            }
            if (svrid != 1){
                for (int i = 0; i < 3; ++i){
                    if (g_dsCtx->m_iSelectedNum[i] == svrid){
                        m_titleWdg->m_btnNumR[i]->show();
                        m_titleWdg->m_btnNumB[i]->hide();
                    }else{
                        m_titleWdg->m_btnNumB[i]->show();
                        m_titleWdg->m_btnNumR[i]->hide();
                    }
                }
            }
            m_titleWdg->show();
        }
    }else{
        m_titleWdg->hide();
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
    g_dsCtx->m_iSelectedNum[num-1] = svrid;
    int wdg[16];
    for (int i = 0; i < 3; ++i){
        int svrid = g_dsCtx->m_iSelectedNum[i];
        wdg[svrid] *= 10;
        wdg[svrid] += i;
    }
}

void HGLWidget::onNumUnselected(int num){
    g_dsCtx->m_iSelectedNum[num-1] = 0;
}

void HGLWidget::mousePressEvent(QMouseEvent* event){
    m_bMousePressed = true;
    m_tmMousePressed = event->timestamp();
    event->ignore();
}

void HGLWidget::mouseReleaseEvent(QMouseEvent* event){
    QRect rc(0, 0, width(), height());
    if (m_bMousePressed && (event->timestamp() - m_tmMousePressed < 300) &&
            rc.contains(event->x(), event->y())){
        toggleToolWidgets();
        emit clicked();
    }

    m_bMousePressed = false;
    event->ignore();
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
    case PICK:
        return &HRcLoader::instance()->tex_pick;
    case PROHIBIT:
        return &HRcLoader::instance()->tex_prohibit;
    case CHANGING:
        return &HRcLoader::instance()->tex_video;
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
            // draw sound icon
//            Texture *tex = getTexture(HAVE_AUDIO);
//            di.left = width() - 32;
//            di.top = 1;
//            di.right = width() - 1;
//            di.bottom = 32;
//            drawTex(tex, &di);

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
        break;
    }

    // draw title
    DsItemInfo* item = g_dsCtx->getItem(svrid);
    if (item && item->title.length() > 0){
        di.left = 2;
        di.top = 2;
        di.color = m_titcolor;
        drawStr(g_dsCtx->m_pFont, item->title.c_str(), &di);
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
    di.left = 1;
    di.top = 1;
    di.right = width() - 1;
    di.bottom = height() - 1;
    di.color = m_outlinecolor;
    drawRect(&di);
}

//===============================================================================

HCockGLWidget::HCockGLWidget(QWidget* parent)
    : HGLWidget(parent)
{
    m_labelDrag = new QLabel(this);
    m_labelDrag->setStyleSheet("border:3px groove #FF8C00");
    m_labelDrag->hide();

    m_wdgTrash = new HChangeColorWidget(this);
    m_wdgTrash->setPixmap(HRcLoader::instance()->icon_trash.scaled(128,128));
    m_wdgTrash->hide();

    QObject::connect( g_dsCtx, SIGNAL(cockChanged()), this, SLOT(onCockChanged()) );
    QObject::connect( this, SIGNAL(clicked()), this, SLOT(toggleTrash()) );
}

HCockGLWidget::~HCockGLWidget(){

}

bool HCockGLWidget::getCockByPos(QPoint pt, QRect& rc){
    for (int i = 1; i < m_vecCocks.size(); ++i){
        if (m_vecCocks[i].contains(pt)){
            rc = m_vecCocks[i];
            return true;
        }
    }

    rc = m_vecCocks[0];
    return false;
}

void HCockGLWidget::onCockChanged(){
    // scale
    double scale_x = width() / (double)g_dsCtx->m_iOriginCockW;
    double scale_y = height() / (double)g_dsCtx->m_iOriginCockH;
    m_vecCocks.clear();
    for (int i = 0; i < g_dsCtx->m_cntCock; ++i){
        int x = g_dsCtx->m_tOriginCocks[i].x * scale_x;
        int y = g_dsCtx->m_tOriginCocks[i].y * scale_y;
        int w = g_dsCtx->m_tOriginCocks[i].w * scale_x;
        int h = g_dsCtx->m_tOriginCocks[i].h * scale_y;
        QRect rc(x,y,w,h);
        m_vecCocks.push_back(rc);
    }
}

void HCockGLWidget::paintGL(){
    HGLWidget::paintGL();

    DrawInfo di;

    // draw taskinfo
    if (g_dsCtx->info && g_dsCtx->m_pFont){
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

    // draw cock sub window outline
    for (int i = 1; i < m_vecCocks.size(); ++i){
        di.left = m_vecCocks[i].left();
        di.top = m_vecCocks[i].top();
        di.right = m_vecCocks[i].right();
        di.bottom = m_vecCocks[i].bottom();
        di.color = m_outlinecolor;
        drawRect(&di);
    }
}

void HCockGLWidget::resizeEvent(QResizeEvent *e){
    onCockChanged();
    m_wdgTrash->setGeometry(width()-128-1, height()/2-64, 128, 128);

    HGLWidget::resizeEvent(e);
}

void HCockGLWidget::mouseMoveEvent(QMouseEvent *e){
    if (status(MAJOR_STATUS_MASK) == PLAYING && !m_labelDrag->isVisible()){
        m_ptDrag.setX(e->x());
        m_ptDrag.setY(e->y());

        QRect rc;
        if ( getCockByPos(m_ptDrag, rc) ){
            m_labelDrag->setFixedSize(rc.size());
            m_labelDrag->setPixmap(grab(rc));
            m_labelDrag->show();
        }else{
            if (m_wdgTrash->isVisible()){
                m_labelDrag->setFixedSize(DRAG_WIDTH, DRAG_HEIGHT);
                m_labelDrag->setPixmap(grab(QRect(m_ptDrag.x()-DRAG_WIDTH/2, m_ptDrag.y()-DRAG_HEIGHT/2, DRAG_WIDTH, DRAG_HEIGHT)) );
                m_labelDrag->show();
            }
        }
    }

    if (m_labelDrag->isVisible()){
        int w = m_labelDrag->width();
        int h = m_labelDrag->height();
        int x1 = e->x()-w/2;
        int y1 = e->y()-h/2;
        int x2 = x1 + w - 1;
        int y2 = y1 + h - 1;

        if (m_wdgTrash->isVisible()){
            if (m_wdgTrash->geometry().contains(e->pos())){
                m_wdgTrash->changeColor(QColor(255, 0, 0, 128));
            }else{
                m_wdgTrash->changeColor(Qt::transparent);
            }
            m_labelDrag->setGeometry(x1,y1,w,h);
        }else{
            if (x1 < 0)
                x1 = 0;
            if (x2 >= width())
                x1 = width()-w-1;
            if (y1 < 0)
                y1 = 0;
            if (y2 >= height())
                y1 = height()-h-1;
            m_labelDrag->setGeometry(x1,y1,w,h);
        }
    }
}

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
            //QRect rc = m_labelDrag->geometry();
        }
    }
}
