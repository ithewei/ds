#ifndef HGLWIDGET_H
#define HGLWIDGET_H

#include <GL/glew.h>
#include <FTGL/ftgl.h>
#include <qopenglwidget.h>
#include <QTimer>
#include "ds_global.h"
#include "htitlebarwidget.h"
#include "htoolbarwidget.h"

#define TITLE_BAR_HEIGHT    50
#define TOOL_BAR_HEIGHT     50

// GL PixelFormat extend
#define GL_I420				0x1910

#define R(color) (((color) >> 24) & 0xFF)
#define G(color) (((color) >> 16) & 0xFF)
#define B(color) (((color) >>  8) & 0xFF)

struct Texture{
    GLuint texID; // glGenTextures分配的ID
    GLuint type; // 数据类型如GL_RGB
    GLint width;
    GLint height;
    GLint bpp;
    GLubyte* data; // 像素数据

    Texture(){
        texID = 0;
        type = GL_RGBA;
        width = 0;
        height = 0;
        bpp = 0;
        data = NULL;
    }

    ~Texture(){
        release();
    }

    void release(){
        if (data != NULL){
            free(data);
            data = NULL;
        }
        width = 0;
        height = 0;
    }
};

struct DrawInfo{
    int left;
    int right;
    int top;
    int bottom;
    GLuint color;
};

enum GLWND_STATUS{
    STOP        =  1,
    PAUSE       =  2,
    PLAYING     =  3,
    NOSIGNAL    =  4,
};

enum GLWDG_ICONS{
    NONE = 0,

    HAVE_AUDIO  = 1,

    PICK        = 10,
    PROHIBIT,
    CHANGING,
};

class HGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    HGLWidget(QWidget* parent = Q_NULLPTR);
    ~HGLWidget();

    int status() {return m_status;}
    void setStatus(int status){
        m_status = status;
        repaint();
    }

    void addIcon(int type, int x, int y, int w, int h);
    void removeIcon(int type);
    Texture* getTexture(int type);

    void setTitle(const char* title) {m_title = title;m_titleWdg->setTitle(title);}
    void setTitleColor(int color) {m_titcolor = color;}
    void setOutlineColor(int color) {m_outlinecolor = color;}

    void showTitlebar(bool bShow = true);
    void showToolbar(bool bShow = true);
    void toggleTitlebar();
    void toggleToolbar();

signals:
    void fullScreen();
    void exitFullScreen();

protected:
    static void loadYUVShader();
    void initVAO();
    void drawYUV(Texture* tex);
    void drawTex(Texture* tex, DrawInfo* di);
    void drawStr(FTGLPixmapFont *pFont, const char* str, DrawInfo* di);
    void drawRect(DrawInfo* di);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void initUI();
    void initConnect();
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

public:
    int svrid;

    static bool s_bInitGLEW;
    static GLuint prog_yuv;
    static GLuint texUniformY;
    static GLuint texUniformU;
    static GLuint texUniformV;

    GLuint tex_yuv[3];
    enum E_VER_ATTR{ver_attr_ver = 3, ver_attr_tex = 4, ver_attr_num};

    HTitlebarWidget* m_titleWdg;
    HToolbarWidget*  m_toolWdg;

    std::string m_title;
    int m_titcolor;
    int m_outlinecolor;

    int m_status;

    std::map<int ,DrawInfo> m_mapIcons; // type : DrawInfo
    tmc_mutex_type m_mutex;

    bool m_bMousePressed;
    ulong m_tmMousePressed;
};

class HCockGLWidget : public HGLWidget
{
    Q_OBJECT
public:
    HCockGLWidget(QWidget* parent = Q_NULLPTR);
    ~HCockGLWidget();

protected:
    virtual void paintGL();
    virtual void resizeEvent(QResizeEvent *e);

private:
    std::vector<QRect> m_vecCocks;
};

#endif // HGLWIDGET_H
