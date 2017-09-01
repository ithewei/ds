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

    hbox->setMargin(1);
    hbox->setSpacing(10);

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    m_btnNum = new QPushButton;
    m_btnNum->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnNum->setIcon(QIcon(HRcLoader::instance()->icon_num));
    m_btnNum->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnNum->setFlat(true);
    hbox->addWidget(m_btnNum);

//    m_btnStartRecord = new QPushButton;
//    m_btnStartRecord->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
//    m_btnStartRecord->setIcon(QIcon(HRcLoader::instance()->icon_record));
//    m_btnStartRecord->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
//    m_btnStartRecord->setFlat(true);
//    hbox->addWidget(m_btnStartRecord);

//    m_btnStopRecord = new QPushButton;
//    m_btnStopRecord->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
//    m_btnStopRecord->setIcon(QIcon(HRcLoader::instance()->icon_recording));
//    m_btnStopRecord->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
//    m_btnStopRecord->setFlat(true);
//    m_btnStopRecord->hide();
//    hbox->addWidget(m_btnStopRecord);

    m_btnSnapshot = new QPushButton;
    m_btnSnapshot->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnSnapshot->setIcon(QIcon(HRcLoader::instance()->icon_snapshot));
    m_btnSnapshot->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnSnapshot->setFlat(true);
    hbox->addWidget(m_btnSnapshot);

    m_btnFullScreen = new QPushButton;
    m_btnFullScreen->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnFullScreen->setIcon(QIcon(HRcLoader::instance()->icon_fullscreen));
    m_btnFullScreen->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnFullScreen->setFlat(true);
    m_btnFullScreen->show();
    hbox->addWidget(m_btnFullScreen);

    m_btnExitFullScreen = new QPushButton;
    m_btnExitFullScreen->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnExitFullScreen->setIcon(QIcon(HRcLoader::instance()->icon_exit_fullscreen));
    m_btnExitFullScreen->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnExitFullScreen->setFlat(true);
    m_btnExitFullScreen->hide();
    hbox->addWidget(m_btnExitFullScreen);

    setLayout(hbox);
}

void HTitlebarWidget::initConnection(){
    QObject::connect( m_btnFullScreen, SIGNAL(clicked()), this, SLOT(onFullScreen()) );
    QObject::connect( m_btnExitFullScreen, SIGNAL(clicked()), this, SLOT(onExitFullScreen()) );

    //QObject::connect( m_btnStartRecord, SIGNAL(clicked(bool)), this, SLOT(onStartRecord()) );
    //QObject::connect( m_btnStopRecord, SIGNAL(clicked(bool)), this, SLOT(onStopRecord()) );
}

void HTitlebarWidget::onFullScreen(){
    m_btnFullScreen->hide();
    m_btnExitFullScreen->show();
    emit fullScreen();
}

void HTitlebarWidget::onExitFullScreen(){
    m_btnExitFullScreen->hide();
    m_btnFullScreen->show();
    emit exitFullScreen();
}

void HTitlebarWidget::onStartRecord(){
    m_btnStartRecord->hide();
    m_btnStopRecord->show();
}

void HTitlebarWidget::onStopRecord(){
    m_btnStartRecord->show();
    m_btnStopRecord->hide();
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
