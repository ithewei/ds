#include "htitlebarwidget.h"
#include "hrcloader.h"
#include "hdsctx.h"

HTitlebarWidget::HTitlebarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HTitlebarWidget::initUI(){
    setBgFg(this, QColor(105,105,105,204),  QColor(255,255,255));

    QHBoxLayout* hbox = genHBoxLayout();
    hbox->setSpacing(g_dsCtx->m_tInit.spacing);

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    QSize sz(g_dsCtx->m_tInit.titlebar_height,g_dsCtx->m_tInit.titlebar_height);

#if LAYOUT_TYPE_OUTPUT_AND_MV
    m_btnNum = genPushButton(sz, HRcLoader::instance()->get(RC_NUM));
    hbox->addWidget(m_btnNum);

    m_btnMicphoneOpened = genPushButton(sz, HRcLoader::instance()->get(RC_MICPHONE));
    m_btnMicphoneOpened->hide();
    hbox->addWidget(m_btnMicphoneOpened);

    m_btnMicphoneClosed = genPushButton(sz, HRcLoader::instance()->get(RC_MICPHONE_GRAY));
    m_btnMicphoneClosed->hide();
    hbox->addWidget(m_btnMicphoneClosed);
#endif

    m_btnVoice = genPushButton(sz, HRcLoader::instance()->get(RC_VOICE));
    m_btnVoice->hide();
    hbox->addWidget(m_btnVoice);

    m_btnMute = genPushButton(sz, HRcLoader::instance()->get(RC_MUTE));
    m_btnMute->hide();
    hbox->addWidget(m_btnMute);

    m_btnSnapshot = genPushButton(sz, HRcLoader::instance()->get(RC_SNAPSHOT));
    m_btnSnapshot->hide();
    hbox->addWidget(m_btnSnapshot);

    m_btnDrawInfo = genPushButton(sz, HRcLoader::instance()->get(RC_INFO));
    m_btnDrawInfo->hide();
    hbox->addWidget(m_btnDrawInfo);

    m_btnFullScreen = genPushButton(sz, HRcLoader::instance()->get(RC_FULLSCREEN));
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = genPushButton(sz, HRcLoader::instance()->get(RC_EXIT_FULLSCREEN));
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

    setLayout(hbox);
}

void HTitlebarWidget::initConnect(){
    connectButtons(m_btnFullScreen, m_btnExitFullScreen);

#if LAYOUT_TYPE_OUTPUT_AND_MV
    connectButtons(m_btnMicphoneOpened, m_btnMicphoneClosed);
#endif

    connectButtons(m_btnVoice, m_btnMute);
}

bool HTitlebarWidget::event(QEvent *e){
    switch (e->type()){
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        return true;
    default:
        return QWidget::event(e);
    }
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

HCombTitlebarWidget::HCombTitlebarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HCombTitlebarWidget::initUI(){
    setBgFg(this, QColor(105,105,105,204),  QColor(255,255,255));

    QHBoxLayout* hbox = genHBoxLayout();
    hbox->setSpacing(g_dsCtx->m_tInit.spacing);

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    QSize sz(g_dsCtx->m_tInit.output_titlebar_height,g_dsCtx->m_tInit.output_titlebar_height);

    m_btnSnapshot = genPushButton(sz, HRcLoader::instance()->get(RC_SNAPSHOT));
    m_btnSnapshot->hide();
    hbox->addWidget(m_btnSnapshot);

    m_btnDrawInfo = genPushButton(sz, HRcLoader::instance()->get(RC_INFO));
    hbox->addWidget(m_btnDrawInfo);

    m_btnPinb = genPushButton(sz, HRcLoader::instance()->get(RC_PINB));
    m_btnPinb->show();
    hbox->addWidget(m_btnPinb);

    m_btnPinr = genPushButton(sz, HRcLoader::instance()->get(RC_PINR));
    m_btnPinr->hide();
    hbox->addWidget(m_btnPinr);

    m_btnFullScreen = genPushButton(sz, HRcLoader::instance()->get(RC_FULLSCREEN));
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = genPushButton(sz, HRcLoader::instance()->get(RC_EXIT_FULLSCREEN));
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

#if LAYOUT_TYPE_ONLY_OUTPUT
    m_btnFullScreen->hide();
    m_btnExitFullScreen->show();
#endif

    setLayout(hbox);
}

void HCombTitlebarWidget::initConnect(){
#if !LAYOUT_TYPE_ONLY_OUTPUT
    connectButtons(m_btnFullScreen, m_btnExitFullScreen);
#endif
    connectButtons(m_btnPinb, m_btnPinr);
}

bool HCombTitlebarWidget::event(QEvent *e){
    switch (e->type()){
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        return true;
    default:
        return QWidget::event(e);
    }
}
