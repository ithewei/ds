#include "hmaintoolbar.h"
#include "hrcloader.h"
#include "hdsctx.h"
#include <QWebChannel>

const char* toolbar_url = "http://localhost/transcoder/audio/index.html";

HWebContext::HWebContext()
 : QObject()
{

}

void HWebContext::setAction(int action){
    g_dsCtx->setAction(action);
}

void HWebContext::toogleInfo(){
    if (g_dsCtx->m_tInit.drawinfo)
        g_dsCtx->m_tInit.drawinfo = 0;
    else
        g_dsCtx->m_tInit.drawinfo = 1;
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

HMainToolbar::HMainToolbar(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
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

    QWebChannel* webchannel = new QWebChannel(m_webview);
    m_webview->page()->setWebChannel(webchannel);
    m_webContext = new HWebContext;
    webchannel->registerObject(QStringLiteral("content"), (QObject*)m_webContext);
}

void HMainToolbar::initConnect(){
    QObject::connect( m_webview, SIGNAL(loadFinished(bool)), m_webview, SLOT(show()) );
}

void HMainToolbar::show(){
    m_webview->setFixedSize(width(), height());
    m_webview->setUrl(QUrl(toolbar_url));
    QWidget::show();
}


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

HStyleToolbar::HStyleToolbar(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}


void HStyleToolbar::initUI(){
    QHBoxLayout* hbox = genHBoxLayout();

    hbox->addStretch();

    QSize sz(90,90);

    m_btnStyle1 = genPushButton(sz, HRcLoader::instance()->icon_style1);
    hbox->addWidget(m_btnStyle1);

    m_btnStyle2 = genPushButton(sz, HRcLoader::instance()->icon_style2);
    hbox->addWidget(m_btnStyle2);

    m_btnStyle4 = genPushButton(sz, HRcLoader::instance()->icon_style4);
    hbox->addWidget(m_btnStyle4);

    m_btnStyle9 = genPushButton(sz, HRcLoader::instance()->icon_style9);
    hbox->addWidget(m_btnStyle9);

    m_btnStyle16 = genPushButton(sz, HRcLoader::instance()->icon_style16);
    hbox->addWidget(m_btnStyle16);

    m_btnMerge = genPushButton(sz, HRcLoader::instance()->icon_merge);
    m_btnMerge->setToolTip(tr("通过鼠标圈选，合并单元格"));
    hbox->addWidget(m_btnMerge);

    hbox->addStretch();

    m_btnReturn = genPushButton(sz, HRcLoader::instance()->icon_return);
    m_btnReturn->setToolTip(tr("返回设置页面"));
    hbox->addWidget(m_btnReturn);

    setLayout(hbox);
}

void HStyleToolbar::initConnect(){
    m_smStyle = new QSignalMapper(this);
    m_smStyle->setMapping(m_btnStyle1, STYLE_1);
    m_smStyle->setMapping(m_btnStyle2, STYLE_2);
    m_smStyle->setMapping(m_btnStyle4, STYLE_4);
    m_smStyle->setMapping(m_btnStyle9, STYLE_9);
    m_smStyle->setMapping(m_btnStyle16, STYLE_16);
    QObject::connect(m_btnStyle1, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle2, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle4, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle9, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle16, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_smStyle, SIGNAL(mapped(int)), this, SLOT(onStyleBtnClicked(int)) );

    QObject::connect(m_btnReturn, SIGNAL(clicked(bool)), this, SLOT(onReturn()) );
}

void HStyleToolbar::leaveEvent(QEvent* e){
    QPoint pt = mapFromGlobal(QCursor::pos());
    if (pt.y() < 1) // leave from top
        hide();
}

void HStyleToolbar::onStyleBtnClicked(int style){
    int row = 1;
    int col = 1;
    switch (style){
    case STYLE_1:
        row = 1; col = 1;
        break;
    case STYLE_2:
        row = 1; col = 2;
        break;
    case STYLE_4:
        row = 2; col = 2;
        break;
    case STYLE_9:
        row = 3; col = 3;
        break;
    case STYLE_16:
        row = 4; col = 4;
        break;
    default:
        break;
    }

    emit styleChanged(row, col);
}

void HStyleToolbar::onReturn(){
    hide();
    g_dsCtx->setAction(0);
}
