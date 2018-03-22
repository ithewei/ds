#include "hmaintoolbar.h"
#include "hrcloader.h"
#include "hdsctx.h"
#include <QWebChannel>

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
      qInfo(ret.toLocal8Bit().constData());

      // when change model, undo status confused, so clear.
      HItemUndo::instance()->clear();
}

#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
HWebView::HWebView(QWidget* parent)
    : QWebEngineView(parent)
{
    m_bAdjustPos = false;

    QObject::connect( this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)) );

//    page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
//    page()->profile()->setHttpCacheType(QWebEngineProfile::NoCache);
//    page()->profile()->clearHttpCache();
//    QWebEngineCookieStore* pCookie = page()->profile()->cookieStore();
//    pCookie->deleteAllCookies();
//    pCookie->deleteSessionCookies();

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void HWebView::onUrlChanged(QUrl url){
    qInfo("url=%s", url.toString().toLocal8Bit().data());

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

        qInfo("w=%d,h=%d", w,h);

        setFixedSize(w,h);
        centerWidget(this);
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

HWebToolbar::HWebToolbar(QWidget *parent) : HWidget(parent){
    need_reload = 0;

    initUI();
    initConnect();

    QWebChannel* webchannel = new QWebChannel(m_webview);
    m_webview->page()->setWebChannel(webchannel);
    m_webContext = new HWebContext;
    webchannel->registerObject(QStringLiteral("content"), (QObject*)m_webContext);
}

#include <QBoxLayout>
void HWebToolbar::initUI(){
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);

    m_webview = new HWebView;
    m_webview->page()->setBackgroundColor(Qt::transparent);
    m_webview->setAutoFillBackground(true);
    QPalette pal = m_webview->palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    m_webview->setPalette(pal);
    m_webview->setUrl(QUrl(dsconf->value("URL/toolbar")));
    hbox->addWidget(m_webview);

    setLayout(hbox);
}

void HWebToolbar::initConnect(){
    //QObject::connect( m_webview, SIGNAL(loadFinished(bool)), m_webview, SLOT(show()) );
}

#include "hdsconf.h"
void HWebToolbar::show(){
    need_reload = 1;
    QString str = dsconf->value("PATH/pic_local");
    str += "comstate.txt";
    FILE* fp = fopen(str.toLocal8Bit().data(), "r+");
    if (fp){
        char buf[32] = {0};
        size_t bytes = fread(buf, 1, 32, fp);
        qInfo("bytes=%d buf=%s", bytes, buf);
        if (bytes != 0 && strncmp(buf, "new", 3) == 0){
            need_reload = 1;
        }else{
            need_reload = 0;
        }
        fseek(fp, 0, SEEK_SET);
        strcpy(buf, "***");
        fwrite(buf, 1, 3, fp);
        fclose(fp);
    }

    if (need_reload){
        m_webview->reload();
    }
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

    m_btnStyle1 = genPushButton(sz, rcloader->get(RC_STYLE1));
    hbox->addWidget(m_btnStyle1);

    m_btnStyle2 = genPushButton(sz, rcloader->get(RC_STYLE2));
    hbox->addWidget(m_btnStyle2);

    m_btnStyle4 = genPushButton(sz, rcloader->get(RC_STYLE4));
    hbox->addWidget(m_btnStyle4);

    m_btnStyle9 = genPushButton(sz, rcloader->get(RC_STYLE9));
    hbox->addWidget(m_btnStyle9);

    m_btnStyle16 = genPushButton(sz, rcloader->get(RC_STYLE16));
    hbox->addWidget(m_btnStyle16);

    m_btnStyle25 = genPushButton(sz, rcloader->get(RC_STYLE25));
    hbox->addWidget(m_btnStyle25);

    m_btnStyle36 = genPushButton(sz, rcloader->get(RC_STYLE36));
    hbox->addWidget(m_btnStyle36);

    m_btnStyle49 = genPushButton(sz, rcloader->get(RC_STYLE49));
    hbox->addWidget(m_btnStyle49);

    m_btnStyle64 = genPushButton(sz, rcloader->get(RC_STYLE64));
    hbox->addWidget(m_btnStyle64);

    m_btnMerge = genPushButton(sz, rcloader->get(RC_MERGE));
    m_btnMerge->setToolTip(STR("通过鼠标圈选，合并单元格!"));
    hbox->addWidget(m_btnMerge);

    hbox->addStretch();

    m_btnReturn = genPushButton(sz, rcloader->get(RC_RETURN));
    m_btnReturn->setToolTip(STR("返回设置页面"));
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
    m_smStyle->setMapping(m_btnStyle25, STYLE_25);
    m_smStyle->setMapping(m_btnStyle36, STYLE_36);
    m_smStyle->setMapping(m_btnStyle49, STYLE_49);
    m_smStyle->setMapping(m_btnStyle64, STYLE_64);
    QObject::connect(m_btnStyle1, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle2, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle4, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle9, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle16, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle25, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle36, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle49, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
    QObject::connect(m_btnStyle64, SIGNAL(clicked(bool)), m_smStyle, SLOT(map()) );
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
    case STYLE_25:
        row = 5; col = 5;
        break;
    case STYLE_36:
        row = 6; col = 6;
        break;
    case STYLE_49:
        row = 7; col = 7;
        break;
    case STYLE_64:
        row = 8; col = 8;
    default:
        break;
    }

    emit styleChanged(row, col);
}

void HStyleToolbar::onReturn(){
    hide();
    g_dsCtx->setAction(0);
}

//===========================================================================

void HModelWidget::paintEvent(QPaintEvent* e){
    QPushButton::paintEvent(e);

    QPainter painter(this);

    QPen pen = painter.pen();

    QFont font = painter.font();
    font.setPixelSize(18);
    painter.setFont(font);

    if (w > 0 && h > 0){
        double xscale = MODEL_WIDTH / (double)w;
        double yscale = MODEL_HEIGHT / (double)h;

        for (int i = 0; i < m_vecRects.size(); i++){
            QRect rc = m_vecRects[i];
            int x = rc.x() * xscale + MODEL_LEFT;
            int y = rc.y() * yscale + MODEL_TOP;
            int w = rc.width() * xscale;
            int h = rc.height() * yscale;
            QRect rcScale(x,y,w,h);
            pen.setColor(Qt::white);
            pen.setWidth(3);
            painter.setPen(pen);
            painter.drawRect(rcScale);
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.drawText(rcScale, Qt::AlignCenter, QString::asprintf("%d", i+1));
        }
    }
}

//=========================================================================================================

void HModelToolbar::initUI(){
    QHBoxLayout* hbox = genHBoxLayout();

    hbox->addStretch();

    model_widgets = new HModelWidget[MAXNUM_MODEL_WIDGET];
    for (int i = 0; i < MAXNUM_MODEL_WIDGET; ++i){
        hbox->addWidget(&(model_widgets[i]));
    }

    setLayout(hbox);

    QSize sz(50,50);
    btn_taskinfo = new QPushButton;
    addClassForModelWidget(btn_taskinfo);
    btn_taskinfo->setIconSize(sz);
    btn_taskinfo->setIcon(rcloader->get(RC_TASKINFO).scaled(sz));
    hbox->addWidget(btn_taskinfo);

    btn_soundmixer = new QPushButton;
    addClassForModelWidget(btn_soundmixer);
    btn_soundmixer->setIconSize(sz);
    btn_soundmixer->setIcon(rcloader->get(RC_SOUNDMIXER).scaled(sz));
    hbox->addWidget(btn_soundmixer);

    btn_home = new QPushButton;
    addClassForModelWidget(btn_home);
    btn_home->setIconSize(sz);
    btn_home->setIcon(rcloader->get(RC_HOME).scaled(sz));
    hbox->addWidget(btn_home);

    updateUI();
}

void HModelToolbar::initConnect(){
    QObject::connect(btn_home, SIGNAL(clicked(bool)), this, SLOT(onBtnHome()));
    QObject::connect(btn_soundmixer, SIGNAL(clicked(bool)), this, SLOT(onBtnSoundmixer()));
    QObject::connect(btn_taskinfo, SIGNAL(clicked(bool)), this, SLOT(onBtnTaskinfo()));
}

void HModelToolbar::updateUI(){
    QSqlQuery query = dsdb->exec(dsconf->value("SQL/query_models"));
    int idx_name = query.record().indexOf("name");
    int idx_width = query.record().indexOf("width");
    int idx_height = query.record().indexOf("height");
    int idx_items = query.record().indexOf("items");
    int i = 0;
    while (query.next()){
        QString name = query.value(idx_name).toString();
        int w = query.value(idx_width).toInt();
        int h = query.value(idx_height).toInt();
        QByteArray items = query.value(idx_items).toByteArray();
        qInfo("name=%s,w=%d,h=%d,items=%s", name.toLocal8Bit().data(), w, h, items.data());
        QJsonDocument doc = QJsonDocument::fromJson(items);
        QJsonArray arr = doc.array();
        HModelWidget* wdg = &model_widgets[i];
        wdg->m_vecRects.clear();
        wdg->w = w;
        wdg->h = h;
        for (int i = 0; i < arr.size(); ++i){
            QJsonObject obj = arr.at(i).toObject();
            int x = obj.value("x").toString().toInt();
            int y = obj.value("y").toString().toInt();
            int w = obj.value("w").toString().toInt();
            int h = obj.value("h").toString().toInt();
            wdg->m_vecRects.push_back(QRect(x,y,w,h));
        }
        wdg->show();
        ++i;
    }

    for (; i < MAXNUM_MODEL_WIDGET; ++i){
        HModelWidget* wdg = &model_widgets[i];
        wdg->hide();
    }
}

void HModelToolbar::onBtnHome(){
    g_dsCtx->setAction(0);
}

void HModelToolbar::onBtnSoundmixer(){
    HWebView* view = new HWebView;
    view->setWindowFlags(Qt::Popup);
    view->setAttribute(Qt::WA_DeleteOnClose, true);
    view->setUrl(QUrl(dsconf->value("URL/soundmixer")));
    int n = 0;
    for (int i = DIRECTOR_SRVID_BEGIN; i <= DIRECTOR_SRVID_END; ++i){
        DsSrvItem* item = g_dsCtx->getSrvItem(i);
        if (item && (item->a_input != 0 || item->v_input != 0))
            ++n;
    }
    view->setFixedSize(QSize(n*160+(n-1)*4, 520));
    centerWidget(view);
    view->show();
}

void HModelToolbar::onBtnTaskinfo(){
    if (g_dsCtx->m_tInit.drawinfo)
        g_dsCtx->m_tInit.drawinfo = 0;
    else
        g_dsCtx->m_tInit.drawinfo = 1;
}
