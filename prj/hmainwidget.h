#ifndef HMAINWIDGET_H
#define HMAINWIDGET_H

#include "qtheaders.h"
#include "hmaintoolbar.h"
#include "hglwidget.h"
#include "hdsctx.h"
#include "hlayout.h"
#include "hsaveinfo.h"

#define DRAG_WIDTH      192
#define DRAG_HEIGHT     108

class HDsContext;
class HMainWidget : public HWidget
{
    Q_OBJECT

    enum EOperate{
        EXCHANGE = 1,
        MERGE = 2,
    };

public:
    explicit HMainWidget(QWidget *parent = nullptr);

    HGLWidget* getGLWdgByPos(QPoint pt);
    HGLWidget* getGLWdgByPos(int x, int y);
    HGLWidget* allocGLWdgForsrvid(int srvid);
    HGLWidget* getGLWdgBysrvid(int srvid);
    HGLWidget* getGLWdgByWndid(int wndid);

protected:
    void initUI();
    void initConnect();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    void updateGLWdgsByLayout();
    void mergeGLWdg(int lt, int rb);

    void updateAllToolWidgets();
    void setExtScreen(int cnt);

public slots:
    void onTimerRepaint();
    void onTimerSave();
    void onActionChanged(int action);
    void onRequestShow(int srvid);
    void onvideoPushed(int srvid, bool bFirstFrame);
    void onAudioPushed(int srvid);
    void onStop(int srvid);
    void onProgressNty(int srvid, int progress);
    void onCombChanged();
    void onVoiceChanged();
    void onFileChanged(QString file);
    void onDirChanged(QString dir);

    void onFullScreen(bool);
    void onGLWdgClicked();

    void onScreenCountChanged(int cnt);
    void onScreenResized(int screen);

#if LAYOUT_TYPE_OUTPUT_AND_INPUT
    void expand();
    void fold();
#endif

    void setLayout(int row, int col);
    void onMerge();

private:
    HLayout m_layout;

private:
    HGLWidget* m_extGLWdg;
    std::vector<HGLWidget*> m_vecGLWdg;
    HGLWidget* m_focusGLWdg;
    HGLWidget* m_fullscreenGLWdg;

#if LAYOUT_TYPE_OUTPUT_AND_INPUT
    QPushButton* m_btnLeftExpand;
    QPushButton* m_btnRightFold;
    HWebToolbar* m_toolbar;
    //HModelToolbar* m_toolbar;
#endif

#if LAYOUT_TYPE_MULTI_INPUT
    HStyleToolbar* m_toolbar;
#endif

    bool m_bMouseMoving;
    QPoint m_ptMousePressed;

    EOperate m_eOperate;
    QLabel* m_labelDrag;
    QLabel* m_labelRect;
    HGLWidget* m_dragSrcWdg;

    QTimer timer_repaint;
    QTimer timer_save;

    QRect m_rcSavedGeometry;
};

#endif // HMAINWIDGET_H
