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
    m_btnStart->setFixedSize(TOOLBAR_ICON_WIDTH,TOOLBAR_ICON_HEIGHT);
    m_btnStart->setIcon(QIcon(HRcLoader::instance()->icon_start));
    m_btnStart->setIconSize(QSize(TOOLBAR_ICON_WIDTH,TOOLBAR_ICON_HEIGHT));
    m_btnStart->setFlat(true);
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);
    hbox->setAlignment(m_btnStart, Qt::AlignLeft);

    m_btnPause = new QPushButton;
    m_btnPause->setFixedSize(TOOLBAR_ICON_WIDTH,TOOLBAR_ICON_HEIGHT);
    m_btnPause->setIcon(QIcon(HRcLoader::instance()->icon_pause));
    m_btnPause->setIconSize(QSize(TOOLBAR_ICON_WIDTH,TOOLBAR_ICON_HEIGHT));
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
    m_slider->setStyleSheet(
                            "QSlider::groove:horizontal {"
                            "   height: 16px;"
                            "   background: #C0C0C0;"
                            "   border: 1px solid green;"
                            "   border-radius: 5px;"
                            "   padding-left:-1px;"
                            "   padding-right:-1px;"
                            "}"
                            "QSlider::sub-page:horizontal"
                            "{"
                                "height: 16px;"
                                "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
                                "background: qlineargradient(x1:0, y1:0.2, x2:1, y2:1, stop:0 #5DCCFF, stop:1 #1874CD);"
                                "border: 1px solid #00FFFF;"
                                "border-radius: 2px;"
                            "}"
                            "QSlider::add-page:horizontal"
                            "{"
                                "height: 16px;"
                                "background: #575757;"
                                "border: 1px solid #00FFFF;"
                                "border-radius: 2px;"
                            "}"
                            "QSlider::handle:horizontal"
                            "{"
                                "width: 16px;"
                                "background: qradialgradient"
                                "("
                                    "spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,"
                                    "stop:0.6 #FFD700, stop:0.778409 #696969"
                                ");"
                                "margin-top: -8px;"
                                "margin-bottom: -8px;"
                                "border-radius: 5px;"
                            "}"
                            );
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

    QObject::connect( m_slider, SIGNAL(actionTriggered(int)), this, SLOT(onSlider(int)) );
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

void HToolbarWidget::onSlider(int action){
    if (action == QAbstractSlider::SliderPageStepAdd ||
        action == QAbstractSlider::SliderPageStepSub){
        qDebug("slider = %d", m_slider->value());
        emit progressChanged(m_slider->value());
    }
}
