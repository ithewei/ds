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

    hbox->setContentsMargins(5,1,5,1);
    hbox->setSpacing(10);

    QSize sz(64,64);

    m_btnStart = new QPushButton;
    m_btnStart->setFixedSize(sz);
    m_btnStart->setIcon(QIcon(HRcLoader::instance()->icon_start));
    m_btnStart->setIconSize(sz);
    m_btnStart->setFlat(true);
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);

    m_btnPause = new QPushButton;
    m_btnPause->setFixedSize(sz);
    m_btnPause->setIcon(QIcon(HRcLoader::instance()->icon_pause));
    m_btnPause->setIconSize(sz);
    m_btnPause->setFlat(true);
    m_btnPause->show();
    hbox->addWidget(m_btnPause);

//    m_btnStop = new QPushButton;
//    m_btnStop->setFixedSize(sz);
//    m_btnStop->setIcon(QIcon(HRcLoader::instance()->icon_stop));
//    m_btnStop->setIconSize(sz);
//    m_btnStop->setFlat(true);
//    hbox->addWidget(m_btnStop);

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
    //hbox->setStretchFactor(m_slider, 1);

    hbox->addSpacing(10);

    setLayout(hbox);
}

void HToolbarWidget::initConnection(){
    QObject::connect( m_btnStart, SIGNAL(clicked(bool)), m_btnStart, SLOT(hide()) );
    QObject::connect( m_btnStart, SIGNAL(clicked(bool)), m_btnPause, SLOT(show()) );

    QObject::connect( m_btnPause, SIGNAL(clicked(bool)), m_btnPause, SLOT(hide()) );
    QObject::connect( m_btnPause, SIGNAL(clicked(bool)), m_btnStart, SLOT(show()) );

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

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HCombToolbarWidget::HCombToolbarWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnection();
}

void HCombToolbarWidget::initUI(){
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(105,105,105,204));
    pal.setColor(QPalette::Foreground, QColor(255,255,255));
    setPalette(pal);

    QHBoxLayout* hbox = new QHBoxLayout;

    hbox->setContentsMargins(5,1,5,1);
    hbox->setSpacing(20);

    QSize sz(64,64);

    m_btnStart = new QPushButton;
    m_btnStart->setFixedSize(sz);
    m_btnStart->setIcon(QIcon(HRcLoader::instance()->icon_start));
    m_btnStart->setIconSize(sz);
    m_btnStart->setFlat(true);
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);

    m_btnPause = new QPushButton;
    m_btnPause->setFixedSize(sz);
    m_btnPause->setIcon(QIcon(HRcLoader::instance()->icon_pause));
    m_btnPause->setIconSize(sz);
    m_btnPause->setFlat(true);
    m_btnPause->show();
    hbox->addWidget(m_btnPause);

//    m_btnStop = new QPushButton;
//    m_btnStop->setFixedSize(sz);
//    m_btnStop->setIcon(QIcon(HRcLoader::instance()->icon_stop));
//    m_btnStop->setIconSize(sz);
//    m_btnStop->setFlat(true);
//    hbox->addWidget(m_btnStop);

    hbox->addStretch();

    m_btnText = new QPushButton;
    m_btnText->setFixedSize(sz);
    m_btnText->setIcon(QIcon(HRcLoader::instance()->icon_text));
    m_btnText->setIconSize(sz);
    m_btnText->setFlat(true);
    hbox->addWidget(m_btnText);

    m_btnExpre = new QPushButton;
    m_btnExpre->setFixedSize(sz);
    m_btnExpre->setIcon(QIcon(HRcLoader::instance()->icon_expre));
    m_btnExpre->setIconSize(sz);
    m_btnExpre->setFlat(true);
    hbox->addWidget(m_btnExpre);

    m_btnZoomOut = new QPushButton;
    m_btnZoomOut->setFixedSize(sz);
    m_btnZoomOut->setIcon(QIcon(HRcLoader::instance()->icon_zoomout));
    m_btnZoomOut->setIconSize(sz);
    m_btnZoomOut->setFlat(true);
    hbox->addWidget(m_btnZoomOut);

    m_btnZoomIn = new QPushButton;
    m_btnZoomIn->setFixedSize(sz);
    m_btnZoomIn->setIcon(QIcon(HRcLoader::instance()->icon_zoomin));
    m_btnZoomIn->setIconSize(sz);
    m_btnZoomIn->setFlat(true);
    hbox->addWidget(m_btnZoomIn);

    m_btnSetting = new QPushButton;
    m_btnSetting->setFixedSize(sz);
    m_btnSetting->setIcon(QIcon(HRcLoader::instance()->icon_setting));
    m_btnSetting->setIconSize(sz);
    m_btnSetting->setFlat(true);
    m_btnSetting->hide();
    hbox->addWidget(m_btnSetting);

    m_btnOK = new QPushButton;
    m_btnOK->setFixedSize(sz);
    m_btnOK->setIcon(QIcon(HRcLoader::instance()->icon_ok));
    m_btnOK->setIconSize(sz);
    m_btnOK->setFlat(true);
    hbox->addWidget(m_btnOK);

    m_btnTrash = new QPushButton;
    m_btnTrash->setFixedSize(sz);
    m_btnTrash->setIcon(QIcon(HRcLoader::instance()->icon_trash));
    m_btnTrash->setIconSize(sz);
    m_btnTrash->setFlat(true);
    hbox->addWidget(m_btnTrash);

    m_btnUndo = new QPushButton;
    m_btnUndo->setFixedSize(sz);
    m_btnUndo->setIcon(QIcon(HRcLoader::instance()->icon_undo));
    m_btnUndo->setIconSize(sz);
    m_btnUndo->setFlat(true);
    hbox->addWidget(m_btnUndo);

    setLayout(hbox);
}

void HCombToolbarWidget::initConnection(){
    QObject::connect( m_btnStart, SIGNAL(clicked(bool)), m_btnStart, SLOT(hide()) );
    QObject::connect( m_btnStart, SIGNAL(clicked(bool)), m_btnPause, SLOT(show()) );

    QObject::connect( m_btnPause, SIGNAL(clicked(bool)), m_btnPause, SLOT(hide()) );
    QObject::connect( m_btnPause, SIGNAL(clicked(bool)), m_btnStart, SLOT(show()) );
}

bool HCombToolbarWidget::event(QEvent *e){
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
