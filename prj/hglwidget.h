#ifndef HGLWIDGET_H
#define HGLWIDGET_H

#include "qglwidgetimpl.h"
#include "ds_global.h"
#include "htitlebarwidget.h"
#include "htoolbarwidget.h"
#include "hnumselectwidget.h"

#define TITLE_BAR_HEIGHT    50
#define TOOL_BAR_HEIGHT     66

#define AUDIO_WIDTH         16
#define AUDIO_HEIGHT        160

enum GLWND_STATUS{
    MAJOR_STATUS_MASK = 0x00FF,
    STOP        =  0x0001,
    PAUSE       =  0x0002,
    PLAYING     =  0x0004,
    NOSIGNAL    =  0x0008,

    MINOR_STATUS_MASK = 0xFF00,
    PLAY_VIDEO  =  0x0100,
    PLAY_AUDIO  =  0x0200,
};

enum GLWDG_ICONS{
    NONE = 0,

    HAVE_AUDIO  = 1,

    PICK        = 10,
    PROHIBIT,
    CHANGING,
};

class HGLWidget : public QGLWidgetImpl
{
    Q_OBJECT
public:
    HGLWidget(QWidget* parent = Q_NULLPTR);
    virtual ~HGLWidget();

    int status(int mask = 0xFFFF) {return m_status & mask;}
    void setStatus(int status, bool bRepaint = true){
        m_status = status;
        if (bRepaint){
            repaint();
        }
    }

    void addIcon(int type, int x, int y, int w, int h);
    void removeIcon(int type);
    Texture* getTexture(int type);

    void setTitle(const char* title) {m_titleWdg->setTitle(title);}
    void setTitleColor(int color) {m_titcolor = color;}
    void setOutlineColor(int color) {m_outlinecolor = color;}

    void showTitlebar(bool bShow = true);
    void showToolbar(bool bShow = true);
    void toggleTitlebar();
    void toggleToolbar();
    virtual void toggleToolWidgets();
    void setProgress(int progress) {m_toolWdg->m_slider->setValue(progress);}

signals:
    void fullScreen();
    void exitFullScreen();
    void clicked();

public slots:
    void snapshot();
    void startRecord();
    void stopRecord();
    void onNumSelected(int num);
    void onNumCanceled(int num);
    void showNumSelector();

    void onStart();
    void onPause();
    void onStop();
    void onProgressChanged(int progress);

protected:
    virtual void paintGL();
    void initUI();
    void initConnect();
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* e);

public:
    int svrid;

    HTitlebarWidget* m_titleWdg;
    HToolbarWidget*  m_toolWdg;
    QLabel* m_snapshot;
    HNumSelectWidget* m_numSelector;

    int m_titcolor;
    int m_outlinecolor;

    int m_status;
    int m_nPreFrame; // previous frame cnt;

    std::map<int ,DrawInfo> m_mapIcons; // type : DrawInfo
    tmc_mutex_type m_mutex;

    bool m_bMousePressed;
    ulong m_tmMousePressed;
};

#include "hchangecolorwidget.h"
class HCockGLWidget : public HGLWidget
{
    Q_OBJECT
public:
    HCockGLWidget(QWidget* parent = Q_NULLPTR);
    ~HCockGLWidget();

    // return is subcock
    bool getCockByPos(QPoint pt, QRect& rc);
    void toggleTrash() {m_wdgTrash->setVisible(!m_wdgTrash->isVisible());}
    virtual void toggleToolWidgets();

public slots:
    void onCockChanged();

protected:
    virtual void paintGL();
    virtual void resizeEvent(QResizeEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    std::vector<QRect> m_vecCocks;

    QLabel* m_labelDrag;
    QPoint m_ptDrag;
    HChangeColorWidget* m_wdgTrash;
};

#endif // HGLWIDGET_H
