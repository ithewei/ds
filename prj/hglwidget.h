#ifndef HGLWIDGET_H
#define HGLWIDGET_H

#include "qglwidgetimpl.h"
#include "ds_global.h"
#include "hdsctx.h"
#include "htitlebarwidget.h"
#include "htoolbarwidget.h"
#include "hnumselectwidget.h"
#include "hnetwork.h"

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

    virtual bool showToolWidgets(bool bShow = true);

signals:
    void fullScreen();
    void exitFullScreen();
    void clicked();

public slots:
    void toggleDrawInfo() {m_bDrawInfo = !m_bDrawInfo;}
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
    int srvid;

    bool m_bDrawInfo;

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
    virtual bool showToolWidgets(bool bShow = true);
    void setProgress(int progress) {m_toolbar->m_slider->setValue(progress);}

public slots:
    void onNumSelected(int num);
    void onNumCanceled(int num);
    void showNumSelector();
    void openMicphone();
    void closeMicphone();
    void onVoice();
    void onMute();

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
#include "haddtextwidget.h"
#include "hoperatetarget.h"

class HCombGLWidget : public HGLWidget
{
    Q_OBJECT
public:
    enum LOCATION{
        NotIn = 0,
        Left = 0x01,
        Right = 0x02,
        Top = 0x04,
        Bottom = 0x08,
        Center = 0x10,
    };
    int getLocation(QPoint pt, QRect rc);


    HCombGLWidget(QWidget* parent = Q_NULLPTR);
    ~HCombGLWidget();

    HOperateTarget* getItemByPos(QPoint pt, HAbstractItem::TYPE type = HAbstractItem::ALL);

    void showTitlebar(bool bShow = true);
    void showToolbar(bool bShow = true);
    virtual bool showToolWidgets(bool bShow = true);

public slots:
    void onCombChanged();
    void onOverlayChanged();

    void lockTools() {m_bLockToolbar = true;}
    void unlockTools() {m_bLockToolbar = false;}

    void onUndo();
    void onTrash();
    void onOK();
    void onZoomIn();
    void onZoomOut();
    void showExpre();
    void onExpreSelected(QString& filepath);
    void showText();
    void onSetting();

protected:
    void initUI();
    void initConnect();

    virtual void drawOutline();
    void drawTaskInfo();
    void drawScreenInfo();
    void drawPictureInfo();
    void drawTextInfo();
    virtual void paintGL();
    virtual void resizeEvent(QResizeEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    void onTargetChanged();
    QRect adjustPos(QRect rc);
    QRect scaleToOrigin(QRect rc);
    QRect scaleToDraw(QRect rc);

    bool showTargetWidget();

private:
    std::vector<HOperateTarget> m_vecScreens;
    std::vector<HOperateTarget> m_vecPictures;
    std::vector<HOperateTarget> m_vecTexts;
    HOperateTarget* m_virtualTarget;

    HOperateTarget* m_target;
    HOperateTarget* m_targetPrev;
    HOperateTarget* m_targetShow;

    HOperateTargetWidget* m_targetWdg;
    HCombTitlebarWidget* m_titlebar;
    HCombToolbarWidget*  m_toolbar;
    HChangeColorWidget* m_wdgTrash;
    HExpreWidget* m_wdgExpre;

    int m_location;
    bool m_bMouseMoving;
    bool m_bLockToolbar;
};
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#endif // HGLWIDGET_H
