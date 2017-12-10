#ifndef HMAINWIDGET_H
#define HMAINWIDGET_H

#include "qtheaders.h"
#include "hmaintoolbar.h"
#include "hglwidget.h"
#include "hdsctx.h"
#include "hlayout.h"

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

    void changeScreenSource(int index, int srvid);
    void addScreenSource(int srvid);

    void updateGLWdgsByLayout();
    void mergeGLWdg(int lt, int rb);

public slots:
    void onTimerRepaint();
    void onActionChanged(int action);
    void onRequestShow(int srvid);
    void onvideoPushed(int srvid, bool bFirstFrame);
    void onAudioPushed(int srvid);
    void onStop(int srvid);
    void onProgressNty(int srvid, int progress);

    void onFullScreen(bool);
    void onGLWdgClicked();

#if LAYOUT_TYPE_OUTPUT_AND_MV
    void expand();
    void fold();
#endif

    void setLayout(int row, int col);
    void onMerge();

private:
    HLayout m_layout;

private:
    std::vector<HGLWidget*> m_vecGLWdg;
    HGLWidget* m_focusGLWdg;

#if LAYOUT_TYPE_OUTPUT_AND_MV
    QPushButton* m_btnLeftExpand;
    QPushButton* m_btnRightFold;
    HMainToolbar* m_toolbar;
#endif

#if LAYOUT_TYPE_ONLY_MV
    HStyleToolbar* m_toolbar;
#endif

    bool m_bMouseMoving;
    QPoint m_ptMousePressed;

    EOperate m_eOperate;
    QLabel* m_labelDrag;
    QLabel* m_labelRect;
    HGLWidget* m_dragSrcWdg;

    QTimer timer_repaint;

    QRect m_rcSavedGeometry;
};

#endif // HMAINWIDGET_H
