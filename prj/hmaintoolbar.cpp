#include "hmaintoolbar.h"
#include "hrcloader.h"
#include "hdsctx.h"
#include <QWebChannel>

const char* toolbar_url = "http://192.168.1.237/transcoder/audio/index";

void HWebContext::setAction(int action){
    g_dsCtx->setAction(action);
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

    m_btnLeftExpand = new QPushButton;
    m_btnLeftExpand->setFixedSize(ICON_WIDTH, ICON_HEIGHT);
    m_btnLeftExpand->setIcon(QIcon(HRcLoader::instance()->icon_left_expand));
    m_btnLeftExpand->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnLeftExpand->setFlat(true);
    m_btnLeftExpand->show();
    hbox->addWidget(m_btnLeftExpand);
    hbox->setAlignment(m_btnLeftExpand, Qt::AlignRight);

    m_btnRightFold = new QPushButton;
    m_btnRightFold->setFixedSize(ICON_WIDTH, ICON_HEIGHT);
    m_btnRightFold->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnRightFold->setIcon(QIcon(HRcLoader::instance()->icon_right_fold));
    m_btnRightFold->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
    m_btnRightFold->setFlat(true);
    m_btnRightFold->hide();
    hbox->addWidget(m_btnRightFold);
    hbox->setAlignment(m_btnRightFold, Qt::AlignRight);

    setLayout(hbox);
}

void HMainToolbar::initConnect(){
    QObject::connect( m_btnLeftExpand, SIGNAL(clicked(bool)), this, SLOT(expand()) );
    QObject::connect( m_btnRightFold, SIGNAL(clicked(bool)), this, SLOT(fold()) );

    QObject::connect( m_webview, SIGNAL(loadFinished(bool)), m_webview, SLOT(show()) );
}

void HMainToolbar::expand(){
    m_btnLeftExpand->hide();
    m_btnRightFold->show();

    m_webview->setFixedWidth(width()-ICON_WIDTH);
    m_webview->setUrl(QUrl(toolbar_url));
}

void HMainToolbar::fold(){
    m_btnLeftExpand->show();
    m_btnRightFold->hide();

    m_webview->hide();
}
