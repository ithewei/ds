#ifndef HMAINWIDGET_H
#define HMAINWIDGET_H

#include "qtheaders.h"
#include "hmaintoolbar.h"
#include "hglwidget.h"
#include "ds_global.h"
#include "hdsctx.h"

#define MAXNUM_GLWIDGET 8

#define DRAG_WIDTH      192
#define DRAG_HEIGHT     108

class HDsContext;
class HMainWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit HMainWidget(HDsContext *ctx, QWidget *parent = nullptr);
    ~HMainWidget();

protected:
    void initUI();
    void initConnect();
    HGLWidget* getGLWdgByPos(QPoint pt);
    HGLWidget* getGLWdgByPos(int x, int y);
    HGLWidget* getGLWdgBysrvid(int srvid);
    HGLWidget* getGLWdgByWndid(int wndid);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:


public slots:
    void onTimerRepaint();
    void onActionChanged(int action);
    void onvideoPushed(int srvid, bool bFirstFrame);
    void onAudioPushed(int srvid);
    void onStop(int srvid);
    void onProgressNty(int srvid, int progress);

    void onFullScreen();
    void onExitFullScreen();
    void onGLWdgClicked();

    void expand();
    void fold();

    void changeScreenSource(int index, int srvid);

private:
    HDsContext* m_ctx;
    std::vector<HGLWidget*> m_vecGLWdg;
    HGLWidget* m_focusGLWdg;

    QPushButton* m_btnLeftExpand;
    QPushButton* m_btnRightFold;
    HMainToolbar* m_toolbar;

    QLabel* m_labelDrag;
    HGLWidget* m_dragSrcWdg;

    QTimer timer_repaint;

    QRect m_rcSavedGeometry;
};

#endif // HMAINWIDGET_H
