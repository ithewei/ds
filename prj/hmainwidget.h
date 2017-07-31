#ifndef HMAINWIDGET_H
#define HMAINWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QPushButton>
#include "hglwidget.h"
#include "ds_global.h"

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
    void initFont();

protected:
    HGLWidget* getGLWdgByPos(int x, int y);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

signals:

public slots:
    /*
    void onStart();
    void onPause();
    */
    void openWeb();
    void onActionChanged(int action);
    void onTitleChanged(int svrid, std::string title);
    void onvideoPushed(int svrid, bool bFirstFrame);
    void onAudioPushed(int svrid);
    void onSourceChanged(int svrid, bool bSucceed);
    void clearOpt();
    void stop(int svrid);

    void onFullScreen();
    void onExitFullScreen();

private:
    HDsContext* m_ctx;
    std::map<int, HGLWidget*> m_mapGLWdg; // svrid : HGLWidget

    QWebEngineView* m_webView;
    HGLWidget* m_dragWdg;

    bool m_bPicked;
    int  m_iClickedSvrid;
    QTimer timer_click;

    QRect m_rcSavedGeometry;
};

#endif // HMAINWIDGET_H
