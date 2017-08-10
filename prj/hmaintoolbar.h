#ifndef HMAINTOOLBAR_H
#define HMAINTOOLBAR_H

#include <QWidget>
#include <QWebEngineView>
#include "ds_global.h"

#define ICON_WIDTH      90
#define ICON_HEIGHT     90

class HWebContext : public QObject{
    Q_OBJECT
public slots:
    void setAction(int action);
};

class HWebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit HWebView(QWidget* parent = 0);
    virtual ~HWebView();

signals:

public slots:
    void onUrlChanged(QUrl url);

protected:
    virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

private:
    bool m_bAdjustPos;
};

class HMainToolbar : public QWidget
{
    Q_OBJECT
public:
    explicit HMainToolbar(QWidget *parent = 0);
    ~HMainToolbar();

signals:


public slots:
    void expand();
    void fold();

protected:
    void initUI();
    void initConnect();

public:
    QPushButton* m_btnLeftExpand;
    QPushButton* m_btnRightFold;
    HWebView* m_webview;
    HWebContext* m_webContext;
};

#endif // HMAINTOOLBAR_H
