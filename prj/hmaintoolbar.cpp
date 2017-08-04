#include "hmaintoolbar.h"
#include <QWebEngineSettings>

HWebView::HWebView(QWidget* parent)
    : QWebEngineView(parent)
{

}

HWebView::~HWebView()
{
    qDebug("");
}

QWebEngineView* HWebView::createWindow(QWebEnginePage::WebWindowType type){
    qDebug("type=%d", type);

    HWebView* view = new HWebView;
    view->setWindowFlags(Qt::Popup);
    view->setAttribute(Qt::WA_DeleteOnClose, true);
    view->setWindowOpacity(0.5);
    int sw  = QApplication::desktop()->width();
    int sh = QApplication::desktop()->height();
    view->setGeometry(sw/4, sh/4, sw/2, sh/2);
    view->show();

    return view;
}

HMainToolbar::HMainToolbar(QWidget *parent) : QWidget(parent)
{
    m_webview = new HWebView(this);
}
