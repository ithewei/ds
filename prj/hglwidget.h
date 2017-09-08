#ifndef HGLWIDGET_H
#define HGLWIDGET_H

#include "qglwidgetimpl.h"
#include "ds_global.h"
#include "htitlebarwidget.h"
#include "htoolbarwidget.h"
#include "hnumselectwidget.h"
#include "hdsctx.h"

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

    void setTitleColor(int color) {m_titcolor = color;}
    void setOutlineColor(int color) {m_outlinecolor = color;}

    virtual void showToolWidgets(bool bShow = true);

signals:
    void fullScreen();
    void exitFullScreen();
    void clicked();

public slots:
    void snapshot();
    void startRecord();
    void stopRecord();

    void onStart();
    void onPause();
    void onStop();

protected:
    virtual void drawVideo();
    virtual void drawAudio();
    virtual void drawIcon();
    virtual void drawTitle();
    virtual void drawOutline();
    virtual void paintGL();

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* e);

public:
    int svrid;

    bool m_bDrawTitle;
    bool m_bDrawAudio;
    bool m_bShowTools;

    QLabel* m_snapshot;

    int m_titcolor;
    int m_outlinecolor;

    int m_status;
    int m_nPreFrame; // previous frame cnt;

    std::map<int ,DrawInfo> m_mapIcons; // type : DrawInfo

    ulong m_tmMousePressed;
    QPoint m_ptMousePressed;
};

class HGeneralGLWidget : public HGLWidget
{
    Q_OBJECT
public:
    HGeneralGLWidget(QWidget* parent = Q_NULLPTR);
    virtual ~HGeneralGLWidget();

    void showTitlebar(bool bShow = true);
    void showToolbar(bool bShow = true);
    virtual void showToolWidgets(bool bShow = true);
    void setProgress(int progress) {m_toolbar->m_slider->setValue(progress);}

public slots:
    void onNumSelected(int num);
    void onNumCanceled(int num);
    void showNumSelector();

    void onProgressChanged(int progress);

protected:
    void drawSelectNum();
    void drawSound();
    virtual void drawOutline();
    virtual void paintGL();
    void initUI();
    void initConnect();

public:
    HTitlebarWidget* m_titlebar;
    HToolbarWidget*  m_toolbar;
    HNumSelectWidget* m_numSelector;
};


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#include "hchangecolorwidget.h"
#include "hexprewidget.h"

struct PictureInfo{
    QRect rc;
};

struct TextInfo{
    QRect rc;
};

struct TimeInfo{
    QRect rc;
};

struct StopwatchInfo{
    QRect rc;
};

class HCombGLWidget : public HGLWidget
{
    Q_OBJECT
public:
    HCombGLWidget(QWidget* parent = Q_NULLPTR);
    ~HCombGLWidget();

    enum LOCATION{
        NotIn = 0,
        Left = 0x01,
        Right = 0x02,
        Top = 0x04,
        Bottom = 0x08,
        Center = 0x10,
    };
    int getLocation(QPoint pt, QRect rc);

    enum COMB_TYPE{
        UNKNOW = 0,
        PIP = 1, //pic in pic 画中画，主画面不能移动和缩放
        TILED = 2, // //平铺，不分主画面和子画面，都能移动和缩放
    };

    enum TRAGET_TYPE{
        NONE = 0,

        SCREEN = 1,
        PICTURE = 2,
        TEXT = 3,
        TIME = 4,
        STOPWATCH = 5,

        LABEL_ADD = 100,

        ALL = 0xFF,
    };

    struct TargetInfo{
        TRAGET_TYPE type;
        int         index;
        int         location;

        TargetInfo(){
            type = NONE;
            index = 0;
            location = Center;
        }
    };

    TargetInfo getTargetByPos(QPoint pt, TRAGET_TYPE type = ALL);
    void showTitlebar(bool bShow = true);
    void showToolbar(bool bShow = true);
    virtual void showToolWidgets(bool bShow = true);
    void adjustPos(QRect& rc);
    void onTargetChanged();

signals:
    void combChanged(DsCombInfo ci);
    void undo();

public slots:
    void onCombChanged();
    void onTrash();
    void onOK();
    void showExpre();
    void onExpreSelected(QString& filepath);

protected:
    void initUI();
    void initConnect();

    virtual void drawOutline();
    virtual void drawTaskInfo();
    virtual void drawCombInfo();
    virtual void paintGL();
    virtual void resizeEvent(QResizeEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    void reposComb(int index, QRect rc);
    void stopComb(int index);

private:
    COMB_TYPE m_combtype;

    std::vector<QRect> m_vecScreens;
    std::vector<PictureInfo> m_vecPictures;
    std::vector<TextInfo> m_vecTexts;
    std::vector<TimeInfo> m_vecTimes;
    std::vector<StopwatchInfo> m_vecStopwatchs;

    TargetInfo m_target;

    bool m_bMouseMoving;
    HCombTitlebarWidget* m_titlebar;
    HCombToolbarWidget*  m_toolbar;
    QLabel* m_labelDrag;
    QPixmap m_pixmapDrag;
    HChangeColorWidget* m_wdgTrash;

    HExpreWidget* m_wdgExpre;
    QLabel* m_labelAdd;
    QPixmap m_pixmapAdd;
};
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#endif // HGLWIDGET_H
