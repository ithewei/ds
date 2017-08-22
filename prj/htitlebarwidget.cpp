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

    m_label = new QLabel;
    hbox->addWidget(m_label);

    hbox->addStretch();

    for (int i = 0; i < 3; ++i){
        m_btnNumB[i] = new QPushButton;
        m_btnNumB[i]->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
        m_btnNumB[i]->setIcon(QIcon(HRcLoader::instance()->icon_numb[i]));
        m_btnNumB[i]->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
        m_btnNumB[i]->setFlat(true);
        m_btnNumB[i]->hide();
        hbox->addWidget(m_btnNumB[i]);

        m_btnNumR[i] = new QPushButton;
        m_btnNumR[i]->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
        m_btnNumR[i]->setIcon(QIcon(HRcLoader::instance()->icon_numr[i]));
        m_btnNumR[i]->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
        m_btnNumR[i]->setFlat(true);
        m_btnNumR[i]->hide();
        hbox->addWidget(m_btnNumR[i]);

        hbox->addSpacing(5);
    }

    hbox->addSpacing(5);

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

//    hbox->addSpacing(10);

    m_btnSnapshot = new QPushButton;
    m_btnSnapshot->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnSnapshot->setIcon(QIcon(HRcLoader::instance()->icon_snapshot));
    m_btnSnapshot->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnSnapshot->setFlat(true);
    hbox->addWidget(m_btnSnapshot);

    hbox->addSpacing(10);

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

    QObject::connect( m_btnNumB[0], SIGNAL(clicked(bool)), this, SLOT(onNumB1()) );
    QObject::connect( m_btnNumR[0], SIGNAL(clicked(bool)), this, SLOT(onNumR1()) );
    QObject::connect( m_btnNumB[1], SIGNAL(clicked(bool)), this, SLOT(onNumB2()) );
    QObject::connect( m_btnNumR[1], SIGNAL(clicked(bool)), this, SLOT(onNumR2()) );
    QObject::connect( m_btnNumB[2], SIGNAL(clicked(bool)), this, SLOT(onNumB3()) );
    QObject::connect( m_btnNumR[2], SIGNAL(clicked(bool)), this, SLOT(onNumR3()) );
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

void HTitlebarWidget::onNumB1(){
    m_btnNumB[0]->hide();
    m_btnNumR[0]->show();
    emit numSelected(1);
}

void HTitlebarWidget::onNumR1(){
    m_btnNumR[0]->hide();
    m_btnNumB[0]->show();
    emit numUnselected(1);
}

void HTitlebarWidget::onNumB2(){
    m_btnNumB[1]->hide();
    m_btnNumR[1]->show();
    emit numSelected(2);
}

void HTitlebarWidget::onNumR2(){
    m_btnNumR[1]->hide();
    m_btnNumB[1]->show();
    emit numUnselected(2);
}

void HTitlebarWidget::onNumB3(){
    m_btnNumB[2]->hide();
    m_btnNumR[2]->show();
    emit numSelected(3);
}

void HTitlebarWidget::onNumR3(){
    m_btnNumR[2]->hide();
    m_btnNumB[2]->show();
    emit numUnselected(3);
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
