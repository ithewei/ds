#ifndef QGLWIDGETIMPL_H
#define QGLWIDGETIMPL_H

#include <GL/glew.h>
#include <FTGL/ftgl.h>
#include <qopenglwidget.h>

// GL PixelFormat extend
#define GL_I420				0x1910

#define R(color) (((color) >> 24) & 0xFF)
#define G(color) (((color) >> 16) & 0xFF)
#define B(color) (((color) >>  8) & 0xFF)
#define A(color) ((color) & 0xFF)

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
        if (texID != 0){
            glDeleteTextures(1,&texID);
        }
    }
};

struct DrawInfo{
    int left;
    int right;
    int top;
    int bottom;
    GLuint color;
};

class QGLWidgetImpl : public QOpenGLWidget
{
    Q_OBJECT
public:
    QGLWidgetImpl(QWidget* parent = Q_NULLPTR);
    virtual ~QGLWidgetImpl();

protected:
    static void loadYUVShader();
    void initVAO();
    void drawYUV(Texture* tex);
    void drawTex(Texture* tex, DrawInfo* di);
    void drawStr(FTGLPixmapFont *pFont, const char* str, DrawInfo* di);
    void drawRect(DrawInfo* di, bool bFill = false);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    //virtual void paintGL();

protected:
    static bool s_bInitGLEW;
    static GLuint prog_yuv;
    static GLuint texUniformY;
    static GLuint texUniformU;
    static GLuint texUniformV;

    GLuint tex_yuv[3];
    enum E_VER_ATTR{ver_attr_ver = 3, ver_attr_tex = 4, ver_attr_num};
};

#endif // QGLWIDGETIMPL_H