#include "htitlebarwidget.h"
#include "ds_global.h"

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
}

void HTitlebarWidget::onFullScreen(){
    m_btnFullScreen->hide();
    m_btnExitFullScreen->show();
    emit fullScreen();
}

void HTitlebarWidget::onExitFullScreen(){
    m_btnFullScreen->show();
    m_btnExitFullScreen->hide();
    emit exitFullScreen();
}

void HTitlebarWidget::mousePressEvent(QMouseEvent* e){
    e->accept();
}

void HTitlebarWidget::mouseReleaseEvent(QMouseEvent* e){
    e->accept();
}
