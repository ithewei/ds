#ifndef HMAINWIDGET_H
#define HMAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "hmaintoolbar.h"
#include "hglwidget.h"
#include "ds_global.h"
#include "hdsctx.h"

#define MAXNUM_GLWIDGET 8

#define DRAG_WIDTH      192
#define DRAG_HEIGHT     108

class HDsContext;
class HMainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HMainWidget(HDsContext *ctx, QWidget *parent = nullptr);
    ~HMainWidget();

public:
    void initUI();
    void initConnect();

protected:
    HGLWidget* getGLWdgByPos(int x, int y);
    HGLWidget* getGLWdgBySvrid(int svrid);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

signals:

public slots:
    void onTimerRepaint();
    void onActionChanged(int action);
    void onTitleChanged(int svrid);
    void onvideoPushed(int svrid, bool bFirstFrame);
    void onAudioPushed(int svrid);
    void onSourceChanged(int svrid, bool bSucceed);
    void onStop(int svrid);
    void onProgressNty(int svrid, int progress);
    void onProgressChanged(int progress);

    void onFullScreen();
    void onExitFullScreen();
    void onGLWdgClicked();

private:
    HDsContext* m_ctx;
    std::vector<HGLWidget*> m_vecGLWdg;
    std::map<int, HGLWidget*> m_mapGLWdg; // svrid : HGLWidget

    HMainToolbar* m_toolbar;

    HGLWidget* m_dragWdg;
    HGLWidget* m_dragSrcWdg;

    QTimer timer_repaint;

    QRect m_rcSavedGeometry;
};

#endif // HMAINWIDGET_H
