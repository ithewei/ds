#include "htitlebarwidget.h"
#include "ds_global.h"
#include "hrcloader.h"

HTitlebarWidget::HTitlebarWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnection();
}

void HTitlebarWidget::initUI(){

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(105,105,105,204));
    pal.setColor(QPalette::Foreground, QColor(255,255,255));
    setPalette(pal);

    QHBoxLayout* hbox = new QHBoxLayout;

    hbox->setContentsMargins(5,1,5,1);
    hbox->setSpacing(20);

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    QSize sz(48,48);

    m_btnNum = new QPushButton;
    m_btnNum->setFixedSize(sz);
    m_btnNum->setIcon(QIcon(HRcLoader::instance()->icon_num));
    m_btnNum->setIconSize(sz);
    m_btnNum->setFlat(true);
    hbox->addWidget(m_btnNum);

    m_btnMicphoneOpened = new QPushButton;
    m_btnMicphoneOpened->setFixedSize(sz);
    m_btnMicphoneOpened->setIcon(QIcon(HRcLoader::instance()->icon_micphone));
    m_btnMicphoneOpened->setIconSize(sz);
    m_btnMicphoneOpened->setFlat(true);
    m_btnMicphoneOpened->hide();
    hbox->addWidget(m_btnMicphoneOpened);

    m_btnMicphoneClosed = new QPushButton;
    m_btnMicphoneClosed->setFixedSize(sz);
    m_btnMicphoneClosed->setIcon(QIcon(HRcLoader::instance()->icon_micphone_gray));
    m_btnMicphoneClosed->setIconSize(sz);
    m_btnMicphoneClosed->setFlat(true);
    m_btnMicphoneClosed->hide();
    hbox->addWidget(m_btnMicphoneClosed);

    m_btnVoice = new QPushButton;
    m_btnVoice->setFixedSize(sz);
    m_btnVoice->setIcon(QIcon(HRcLoader::instance()->icon_voice));
    m_btnVoice->setIconSize(sz);
    m_btnVoice->setFlat(true);
    m_btnVoice->show();
    hbox->addWidget(m_btnVoice);

    m_btnMute = new QPushButton;
    m_btnMute->setFixedSize(sz);
    m_btnMute->setIcon(QIcon(HRcLoader::instance()->icon_mute));
    m_btnMute->setIconSize(sz);
    m_btnMute->setFlat(true);
    m_btnMute->hide();
    hbox->addWidget(m_btnMute);

    m_btnSnapshot = new QPushButton;
    m_btnSnapshot->setFixedSize(sz);
    m_btnSnapshot->setIcon(QIcon(HRcLoader::instance()->icon_snapshot));
    m_btnSnapshot->setIconSize(sz);
    m_btnSnapshot->setFlat(true);
    m_btnSnapshot->hide();
    hbox->addWidget(m_btnSnapshot);

    m_btnDrawInfo = new QPushButton;
    m_btnDrawInfo->setFixedSize(sz);
    m_btnDrawInfo->setIcon(QIcon(HRcLoader::instance()->icon_info.scaled(sz)));
    m_btnDrawInfo->setIconSize(sz);
    m_btnDrawInfo->setFlat(true);
    m_btnDrawInfo->hide();
    hbox->addWidget(m_btnDrawInfo);

    m_btnFullScreen = new QPushButton;
    m_btnFullScreen->setFixedSize(sz);
    m_btnFullScreen->setIcon(QIcon(HRcLoader::instance()->icon_fullscreen));
    m_btnFullScreen->setIconSize(sz);
    m_btnFullScreen->setFlat(true);
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = new QPushButton;
    m_btnExitFullScreen->setFixedSize(sz);
    m_btnExitFullScreen->setIcon(QIcon(HRcLoader::instance()->icon_exit_fullscreen));
    m_btnExitFullScreen->setIconSize(sz);
    m_btnExitFullScreen->setFlat(true);
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

    setLayout(hbox);
}

void HTitlebarWidget::initConnection(){
    QObject::connect( m_btnFullScreen, SIGNAL(clicked()), m_btnFullScreen, SLOT(hide()) );
    QObject::connect( m_btnFullScreen, SIGNAL(clicked()), m_btnExitFullScreen, SLOT(show()) );

    QObject::connect( m_btnExitFullScreen, SIGNAL(clicked()), m_btnExitFullScreen, SLOT(hide()) );
    QObject::connect( m_btnExitFullScreen, SIGNAL(clicked()), m_btnFullScreen, SLOT(show()) );

    QObject::connect( m_btnMicphoneOpened, SIGNAL(clicked()), m_btnMicphoneOpened, SLOT(hide()) );
    QObject::connect( m_btnMicphoneOpened, SIGNAL(clicked()), m_btnMicphoneClosed, SLOT(show()) );

    QObject::connect( m_btnMicphoneClosed, SIGNAL(clicked()), m_btnMicphoneClosed, SLOT(hide()) );
    QObject::connect( m_btnMicphoneClosed, SIGNAL(clicked()), m_btnMicphoneOpened, SLOT(show()) );

    QObject::connect( m_btnVoice, SIGNAL(clicked()), m_btnVoice, SLOT(hide()) );
    QObject::connect( m_btnVoice, SIGNAL(clicked()), m_btnMute, SLOT(show()) );

    QObject::connect( m_btnMute, SIGNAL(clicked()), m_btnMute, SLOT(hide()) );
    QObject::connect( m_btnMute, SIGNAL(clicked()), m_btnVoice, SLOT(show()) );
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

HCombTitlebarWidget::HCombTitlebarWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnection();
}

void HCombTitlebarWidget::initUI(){

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(105,105,105,204));
    pal.setColor(QPalette::Foreground, QColor(255,255,255));
    setPalette(pal);

    QHBoxLayout* hbox = new QHBoxLayout;

    hbox->setContentsMargins(5,1,5,1);
    hbox->setSpacing(10);

    m_label = new QLabel;
    hbox->addWidget(m_label);
    hbox->setAlignment(m_label, Qt::AlignVCenter);

    hbox->addStretch();

    QSize sz(64, 64);

    m_btnSnapshot = new QPushButton;
    m_btnSnapshot->setFixedSize(sz);
    m_btnSnapshot->setIcon(QIcon(HRcLoader::instance()->icon_snapshot.scaled(sz)));
    m_btnSnapshot->setIconSize(sz);
    m_btnSnapshot->setFlat(true);
    m_btnSnapshot->hide();
    hbox->addWidget(m_btnSnapshot);

    m_btnDrawInfo = new QPushButton;
    m_btnDrawInfo->setFixedSize(sz);
    m_btnDrawInfo->setIcon(QIcon(HRcLoader::instance()->icon_info));
    m_btnDrawInfo->setIconSize(sz);
    m_btnDrawInfo->setFlat(true);
    hbox->addWidget(m_btnDrawInfo);

    m_btnFullScreen = new QPushButton;
    m_btnFullScreen->setFixedSize(sz);
    m_btnFullScreen->setIcon(QIcon(HRcLoader::instance()->icon_fullscreen.scaled(sz)));
    m_btnFullScreen->setIconSize(sz);
    m_btnFullScreen->setFlat(true);
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = new QPushButton;
    m_btnExitFullScreen->setFixedSize(sz);
    m_btnExitFullScreen->setIcon(QIcon(HRcLoader::instance()->icon_exit_fullscreen.scaled(sz)));
    m_btnExitFullScreen->setIconSize(sz);
    m_btnExitFullScreen->setFlat(true);
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

    setLayout(hbox);
}

void HCombTitlebarWidget::initConnection(){
    QObject::connect( m_btnFullScreen, SIGNAL(clicked()), m_btnFullScreen, SLOT(hide()) );
    QObject::connect( m_btnFullScreen, SIGNAL(clicked()), m_btnExitFullScreen, SLOT(show()) );

    QObject::connect( m_btnExitFullScreen, SIGNAL(clicked()), m_btnExitFullScreen, SLOT(hide()) );
    QObject::connect( m_btnExitFullScreen, SIGNAL(clicked()), m_btnFullScreen, SLOT(show()) );
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
