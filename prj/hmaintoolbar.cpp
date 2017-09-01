#include "hmaintoolbar.h"
#include "hrcloader.h"
#include "hdsctx.h"
#include <QWebChannel>

const char* toolbar_url = "http://localhost/transcoder/audio/index.html";
//const char* toolbar_url = "https://www.baidu.com";

HWebContext::HWebContext()
 : QObject()
{

}

void HWebContext::setAction(int action){
    g_dsCtx->setAction(action);
}

void HWebContext::toogleInfo(){
    if (g_dsCtx->m_tInit.info)
        g_dsCtx->m_tInit.info = 0;
    else
        g_dsCtx->m_tInit.info = 1;
}

void HWebContext::getSelectInfo(int id){
      QString ret;
      ret = "{";
      for (int i = 0; i < MAX_NUM_ICON; ++i){
          char kv[16];
          snprintf(kv, 16, "\"id%d\":%d", i+1, g_dsCtx->m_preselect[i]);
          ret += kv;
          if (i != MAX_NUM_ICON-1){
              ret += ",";
          }
      }
      ret += "}";
      emit sendSelectInfo(ret, id);
      qDebug(ret.toLocal8Bit().constData());
}

HWebView::HWebView(QWidget* parent)
    : QWebEngineView(parent)
{
    m_bAdjustPos = false;

    QObject::connect( this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)) );
}

HWebView::~HWebView()
{
    qDebug("");
}

void HWebView::onUrlChanged(QUrl url){
    qDebug("url=%s", url.toString().toLocal8Bit().data());

    if (m_bAdjustPos){
        const char* szUrl = url.toString().toLocal8Bit().data();
        const char* szWidth = strstr(szUrl, "width=");
        const char* szHeight = strstr(szUrl, "height=");
        long w = width();
        long h = height();
        if (szWidth){
            w = strtol(szWidth+strlen("width="), NULL, 10);
        }

        if (szHeight){
            h = strtol(szHeight+strlen("height="), NULL, 10);
        }

        qDebug("w=%d,h=%d", w,h);

        int sw = QApplication::desktop()->width();
        int sh = QApplication::desktop()->height();
        if (w < sw && h < sh){
            setGeometry((sw-w)/2, (sh-h)/2, w, h);
        }
    }
}

QWebEngineView* HWebView::createWindow(QWebEnginePage::WebWindowType type){
    qDebug("type=%d", type);

    HWebView* view = new HWebView;
    view->setWindowFlags(Qt::Popup);
    view->setAttribute(Qt::WA_DeleteOnClose, true);
    view->m_bAdjustPos = true;
    view->show();

    return view;
}

HMainToolbar::HMainToolbar(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();

    QWebChannel* webchannel = new QWebChannel(m_webview);
    m_webview->page()->setWebChannel(webchannel);
    m_webContext = new HWebContext;
    webchannel->registerObject(QStringLiteral("content"), (QObject*)m_webContext);
}

HMainToolbar::~HMainToolbar(){
    if (m_webContext){
        delete m_webContext;
        m_webContext = NULL;
    }
}

#include <QBoxLayout>
void HMainToolbar::initUI(){
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);

    m_webview = new HWebView;
    m_webview->page()->setBackgroundColor(Qt::transparent);
    m_webview->setAutoFillBackground(true);
    QPalette pal = m_webview->palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    m_webview->setPalette(pal);
    m_webview->hide();
    hbox->addWidget(m_webview);
    hbox->setAlignment(m_webview, Qt::AlignHCenter);

    setLayout(hbox);
}

void HMainToolbar::initConnect(){
    QObject::connect( m_webview, SIGNAL(loadFinished(bool)), m_webview, SLOT(show()) );
}

void HMainToolbar::show(){
    m_webview->setFixedSize(width(), height());
    m_webview->setUrl(QUrl(toolbar_url));
    QWidget::show();
}
