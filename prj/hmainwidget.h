#ifndef HMAINWIDGET_H
#define HMAINWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QPushButton>
#include "hglwidget.h"
#include "ds_global.h"
#include "hdsctx.h"

#define MAXNUM_GLWIDGET 8
#define ICON_WIDTH      64
#define ICON_HEIGHT     64
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
    void onBack();
    void onStart();
    void onPause();
    void openWeb();
    void onActionChanged(int action);
    void onTitleChanged(int svrid, std::string title);
    void onvideoPushed(int svrid, bool bFirstFrame);
    void onAudioPushed(int svrid);
    void onSourceChanged(int svrid, bool bSucceed);
    void onClicked(int svrid, int x, int y);
    void onDclicked(int svrid, int x, int y);
    void clearOpt();
    void stop(int svrid);

private:
    HDsContext* m_ctx;
    std::map<int, HGLWidget*> m_mapGLWdg; // svrid : HGLWidget

    QWebEngineView* m_webView;
    HGLWidget* m_dragWdg;

    QPushButton* m_btnBack;
    QRect m_rcBack;
    std::string m_strBack;

    QPushButton* m_btnStart;
    QPushButton* m_btnPause;
    QRect m_rcStart;
    QRect m_rcPause;
    std::string m_strStart;
    std::string m_strPause;

    QPushButton* m_btnBrower;
    QRect m_rcBrower;
    std::string m_strBrower;

    bool m_bPicked;
    int  m_iClickedSvrid;
    QTimer timer_click;
};

#endif // HMAINWIDGET_H
