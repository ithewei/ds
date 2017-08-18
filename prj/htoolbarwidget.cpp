#include "htoolbarwidget.h"
#include "ds_global.h"
#include "hrcloader.h"

HToolbarWidget::HToolbarWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnection();
}

void HToolbarWidget::initUI(){
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(105,105,105,204));
    pal.setColor(QPalette::Foreground, QColor(255,255,255));
    setPalette(pal);

    QHBoxLayout* hbox = new QHBoxLayout;

    hbox->setMargin(1);

    m_btnStart = new QPushButton;
    m_btnStart->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnStart->setIcon(QIcon(HRcLoader::instance()->icon_start));
    m_btnStart->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnStart->setFlat(true);
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);
    hbox->setAlignment(m_btnStart, Qt::AlignLeft);

    m_btnPause = new QPushButton;
    m_btnPause->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
    m_btnPause->setIcon(QIcon(HRcLoader::instance()->icon_pause));
    m_btnPause->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
    m_btnPause->setFlat(true);
    m_btnPause->show();
    hbox->addWidget(m_btnPause);
    hbox->setAlignment(m_btnPause, Qt::AlignLeft);

    hbox->addSpacing(10);

//    m_btnStop = new QPushButton;
//    m_btnStop->setFixedSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT);
//    m_btnStop->setIcon(QIcon(HRcLoader::instance()->icon_stop));
//    m_btnStop->setIconSize(QSize(TITLEBAR_ICON_WIDTH,TITLEBAR_ICON_HEIGHT));
//    m_btnStop->setFlat(true);
//    hbox->addWidget(m_btnStop);
//    hbox->setAlignment(m_btnStop, Qt::AlignLeft);

//    hbox->addSpacing(10);

    m_slider = new QSlider;
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setRange(0,100);
    m_slider->setFixedHeight(TITLEBAR_ICON_HEIGHT);
    hbox->addWidget(m_slider);
    hbox->setStretchFactor(m_slider, 1);

    hbox->addSpacing(10);

    hbox->addStretch();

    setLayout(hbox);
}

void HToolbarWidget::initConnection(){
    QObject::connect( m_btnStart, SIGNAL(clicked(bool)), this, SLOT(onStart()) );
    QObject::connect( m_btnPause, SIGNAL(clicked(bool)), this, SLOT(onPause()) );
    //QObject::connect( m_btnStop, SIGNAL(clicked(bool)), this, SLOT(onStop()) );

    QObject::connect( m_slider, SIGNAL(sliderReleased()), this, SLOT(onSlider()) );
}

#include <QEvent>
bool HToolbarWidget::event(QEvent *e){
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

void HToolbarWidget::onStart(){
    m_btnStart->hide();
    m_btnPause->show();

    emit sigStart();
}

void HToolbarWidget::onPause(){
    m_btnStart->show();
    m_btnPause->hide();

    emit sigPause();
}

void HToolbarWidget::onStop(){
    //m_btnStop->setEnabled(false);
    emit sigStop();
}

void HToolbarWidget::onSlider(){
    emit progressChanged(m_slider->value());
}
