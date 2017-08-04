#ifndef HMAINTOOLBAR_H
#define HMAINTOOLBAR_H

#include <QWidget>
#include <QWebEngineView>

class HWebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit HWebView(QWidget* parent = 0);
    virtual ~HWebView();

signals:

public slots:

protected:
    virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
};

class HMainToolbar : public QWidget
{
    Q_OBJECT
public:
    explicit HMainToolbar(QWidget *parent = 0);

signals:

public slots:

public:
    HWebView* m_webview;
};

#endif // HMAINTOOLBAR_H
