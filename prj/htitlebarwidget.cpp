#include "htitlebarwidget.h"
#include "ds_global.h"
#include "hrcloader.h"

HTitlebarWidget::HTitlebarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HTitlebarWidget::initUI(){
    setBgFg(this, QColor(105,105,105,204),  QColor(255,255,255));

    QHBoxLayout* hbox = genHBoxLayout();

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    QSize sz(48,48);

#if LAYOUT_TYPE_OUTPUT_AND_MV
    m_btnNum = genPushButton(sz, HRcLoader::instance()->icon_num);
    hbox->addWidget(m_btnNum);

    m_btnMicphoneOpened = genPushButton(sz, HRcLoader::instance()->icon_micphone);
    m_btnMicphoneOpened->hide();
    hbox->addWidget(m_btnMicphoneOpened);

    m_btnMicphoneClosed = genPushButton(sz, HRcLoader::instance()->icon_micphone_gray);
    m_btnMicphoneClosed->hide();
    hbox->addWidget(m_btnMicphoneClosed);
#endif

    m_btnVoice = genPushButton(sz, HRcLoader::instance()->icon_voice);
    m_btnVoice->hide();
    hbox->addWidget(m_btnVoice);

    m_btnMute = genPushButton(sz, HRcLoader::instance()->icon_mute);
    m_btnMute->hide();
    hbox->addWidget(m_btnMute);

    m_btnSnapshot = genPushButton(sz, HRcLoader::instance()->icon_snapshot);
    m_btnSnapshot->hide();
    hbox->addWidget(m_btnSnapshot);

    m_btnDrawInfo = genPushButton(sz, HRcLoader::instance()->icon_info);
    m_btnDrawInfo->hide();
    hbox->addWidget(m_btnDrawInfo);

    m_btnFullScreen = genPushButton(sz, HRcLoader::instance()->icon_fullscreen);
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = genPushButton(sz, HRcLoader::instance()->icon_exit_fullscreen);
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

    setLayout(hbox);
}

void HTitlebarWidget::initConnect(){
    connectButtons(m_btnFullScreen, m_btnExitFullScreen);

#if LAYOUT_TYPE_OUTPUT_AND_MV
    connectButtons(m_btnMicphoneOpened, m_btnMicphoneOpened);
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

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    QSize sz(64,64);

    m_btnSnapshot = genPushButton(sz, HRcLoader::instance()->icon_snapshot);
    m_btnSnapshot->hide();
    hbox->addWidget(m_btnSnapshot);

    m_btnDrawInfo = genPushButton(sz, HRcLoader::instance()->icon_info);
    m_btnDrawInfo->hide();
    hbox->addWidget(m_btnDrawInfo);

    m_btnPinb = genPushButton(sz, HRcLoader::instance()->icon_pinb);
    m_btnPinb->show();
    hbox->addWidget(m_btnPinb);

    m_btnPinr = genPushButton(sz, HRcLoader::instance()->icon_pinr);
    m_btnPinr->hide();
    hbox->addWidget(m_btnPinr);

    m_btnFullScreen = genPushButton(sz, HRcLoader::instance()->icon_fullscreen);
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = genPushButton(sz, HRcLoader::instance()->icon_exit_fullscreen);
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

    setLayout(hbox);
}

void HCombTitlebarWidget::initConnect(){
    connectButtons(m_btnFullScreen, m_btnExitFullScreen);
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
