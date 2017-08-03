#include "ds_global.h"
#include "hglwidget.h"

bool HGLWidget::s_bInitGLEW = false;
GLuint HGLWidget::prog_yuv;
GLuint HGLWidget::texUniformY;
GLuint HGLWidget::texUniformU;
GLuint HGLWidget::texUniformV;

HGLWidget::HGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
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
    m_titleWdg = new HTitlebarWidget(this);
    m_titleWdg->hide();

    m_toolWdg = new HToolbarWidget(this);
    m_toolWdg->hide();
}

void HGLWidget::initConnect(){
    QObject::connect( m_titleWdg, SIGNAL(fullScreen()), this, SIGNAL(fullScreen()) );
    QObject::connect( m_titleWdg, SIGNAL(exitFullScreen()), this, SIGNAL(exitFullScreen()) );
}

void HGLWidget::showTitlebar(bool bShow){
    if (bShow){
        m_titleWdg->setGeometry(2, 2, width()-4, TITLE_BAR_HEIGHT);
        m_titleWdg->show();
    }else{
        m_titleWdg->hide();
    }
}

void HGLWidget::showToolbar(bool bShow){
    if (bShow){
        m_toolWdg->setGeometry(2, height()-TOOL_BAR_HEIGHT-2, width()-4, TITLE_BAR_HEIGHT);
        m_toolWdg->show();
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

void HGLWidget::mousePressEvent(QMouseEvent* event){
    qDebug("%d,%d", event->x(), event->y());

    m_bMousePressed = true;
    m_tmMousePressed = event->timestamp();
    event->ignore();
}

void HGLWidget::mouseReleaseEvent(QMouseEvent* event){
    qDebug("%d,%d", event->x(), event->y());

    QRect rc(0, 0, width(), height());
    if (m_bMousePressed && (event->timestamp() - m_tmMousePressed < 300) &&
            rc.contains(event->x(), event->y())){
        toggleTitlebar();
        toggleToolbar();
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

void HGLWidget::initializeGL(){
    qDebug("");
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

void HGLWidget::resizeGL(int w, int h){
    qDebug("");
    glViewport(0, 0, w, h);

    showTitlebar(false);
    showToolbar(false);

    m_mapIcons.clear();
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
    case PAUSE:
        //
        break;
    case NOSIGNAL:
        di.left = width()/2 - 50;
        di.top = height()/2 - 20;
        di.color = 0xFFFFFFFF;
        drawStr(g_dsCtx->m_pFont, "NO SIGNAL!", &di);
        break;
    case PLAYING:
        if (m_status & PLAY_VIDEO){
            // draw yuv
            if (g_dsCtx->tex_yuv[svrid].data && g_dsCtx->tex_yuv[svrid].width > 0
                    && g_dsCtx->tex_yuv[svrid].height > 0){
                drawYUV(&g_dsCtx->tex_yuv[svrid]);
            }
        }

        if (m_status & PLAY_AUDIO){
            // draw sound icon
            Texture *tex = getTexture(HAVE_AUDIO);
            di.left = width() - 32;
            di.top = 1;
            di.right = width() - 1;
            di.bottom = 32;
            drawTex(tex, &di);
        }
        break;
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

    // draw title
    if (m_title.length() > 0){
        di.left = 2;
        di.top = 2;
        di.color = m_titcolor;
        drawStr(g_dsCtx->m_pFont, m_title.c_str(), &di);
    }

    // draw outline
    di.left = 1;
    di.top = 1;
    di.right = width() - 1;
    di.bottom = height() - 1;
    di.color = m_outlinecolor;
    drawRect(&di);
}

void HGLWidget::loadYUVShader(){
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
}

void HGLWidget::initVAO(){
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void HGLWidget::drawYUV(Texture* tex){
    glUseProgram(prog_yuv);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

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
}

void HGLWidget::drawTex(Texture* tex, DrawInfo* di){
    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->texID);
    gluBuild2DMipmaps(GL_TEXTURE_2D, tex->bpp/8, tex->width, tex->height, tex->type, GL_UNSIGNED_BYTE, tex->data);

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

void HGLWidget::drawStr(FTGLPixmapFont *pFont, const char* str, DrawInfo* di){
    if (!pFont)
        return ;

    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    const char* mbs = str;
    int len_mbs = strlen(mbs);
#ifdef WIN32
    setlocale(LC_ALL,".936");
#else
    setlocale(LC_ALL,"zh_CN.utf8");
#endif
    int len_wcs = mbstowcs(NULL, mbs, 0);
    wchar_t* wcs = new wchar_t[len_wcs + 1];
    mbstowcs(wcs, mbs, strlen(mbs)+1);

    glColor3ub(R(di->color), G(di->color), B(di->color));
    glRasterPos2i(di->left, di->top + pFont->LineHeight());
    pFont->Render(wcs);
    glColor3ub(255,255,255);

    delete[] wcs;
}

void HGLWidget::drawRect(DrawInfo* di){
    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3ub(R(di->color), G(di->color), B(di->color));
    glRecti(di->left, di->top, di->right, di->bottom);
    glColor3ub(255,255,255);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//===============================================================================

HCockGLWidget::HCockGLWidget(QWidget* parent)
    : HGLWidget(parent)
{

}

HCockGLWidget::~HCockGLWidget(){

}

void HCockGLWidget::paintGL(){
    HGLWidget::paintGL();

    // draw cock sub window outline
    DrawInfo di;

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
    QSize sz = e->size();

    qDebug("%d*%d", sz.width(),sz.height());

    // scale
    double scale_x = sz.width() / (double)g_dsCtx->m_iOriginCockW;
    double scale_y = sz.height() / (double)g_dsCtx->m_iOriginCockH;
    m_vecCocks.clear();
    for (int i = 0; i < g_dsCtx->m_cntCock; ++i){
        int x = g_dsCtx->m_tOriginCocks[i].x * scale_x;
        int y = g_dsCtx->m_tOriginCocks[i].y * scale_y;
        int w = g_dsCtx->m_tOriginCocks[i].w * scale_x;
        int h = g_dsCtx->m_tOriginCocks[i].h * scale_y;
        QRect rc(x,y,w,h);
        m_vecCocks.push_back(rc);
    }

    HGLWidget::resizeEvent(e);
}
