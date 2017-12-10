#include "htoolbarwidget.h"
#include "hrcloader.h"
#include "hdsctx.h"

HToolbarWidget::HToolbarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HToolbarWidget::initUI(){
    setBgFg(this, QColor(105,105,105,204), QColor(255,255,255));

    QHBoxLayout* hbox = genHBoxLayout();
    hbox->setSpacing(g_dsCtx->m_tInit.spacing);

    QSize sz(g_dsCtx->m_tInit.toolbar_height, g_dsCtx->m_tInit.toolbar_height);

    m_btnStart = genPushButton(sz, HRcLoader::instance()->get(RC_START));
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);

    m_btnPause = genPushButton(sz, HRcLoader::instance()->get(RC_PAUSE));
    m_btnPause->show();
    hbox->addWidget(m_btnPause);

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

    hbox->addSpacing(10);

    setLayout(hbox);
}

void HToolbarWidget::initConnect(){
    connectButtons(m_btnStart, m_btnPause);

    QObject::connect( m_slider, SIGNAL(actionTriggered(int)), this, SLOT(onSlider(int)) );
    QObject::connect( m_slider, SIGNAL(sliderReleased()), this, SLOT(onSlider()) );
}

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
HCombToolbarWidget::HCombToolbarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HCombToolbarWidget::initUI(){
    setBgFg(this, QColor(105,105,105,204), QColor(255,255,255));

    QHBoxLayout* hbox = genHBoxLayout();
    hbox->setSpacing(g_dsCtx->m_tInit.spacing);

    QSize sz(g_dsCtx->m_tInit.output_toolbar_height, g_dsCtx->m_tInit.output_toolbar_height);

    m_btnStart = genPushButton(sz, HRcLoader::instance()->get(RC_START));
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);

    m_btnPause = genPushButton(sz, HRcLoader::instance()->get(RC_PAUSE));
    m_btnPause->show();
    hbox->addWidget(m_btnPause);

    hbox->addStretch();

    m_btnText = genPushButton(sz, HRcLoader::instance()->get(RC_TEXT));
    hbox->addWidget(m_btnText);

    m_btnExpre = genPushButton(sz, HRcLoader::instance()->get(RC_EXPRE));
    hbox->addWidget(m_btnExpre);

    m_btnEffect = genPushButton(sz, HRcLoader::instance()->get(RC_EFFECT));
    m_btnEffect->hide();
    hbox->addWidget(m_btnEffect);

    m_btnZoomOut = genPushButton(sz, HRcLoader::instance()->get(RC_ZOOMOUT));
    hbox->addWidget(m_btnZoomOut);

    m_btnZoomIn = genPushButton(sz, HRcLoader::instance()->get(RC_ZOOMIN));
    hbox->addWidget(m_btnZoomIn);

    m_btnSetting = genPushButton(sz, HRcLoader::instance()->get(RC_SETTING));
    m_btnSetting->hide();
    hbox->addWidget(m_btnSetting);

    m_btnOK = genPushButton(sz, HRcLoader::instance()->get(RC_OK));
    hbox->addWidget(m_btnOK);

    m_btnTrash = genPushButton(sz, HRcLoader::instance()->get(RC_TRASH));
    hbox->addWidget(m_btnTrash);

    m_btnUndo = genPushButton(sz, HRcLoader::instance()->get(RC_UNDO));
    hbox->addWidget(m_btnUndo);

    setLayout(hbox);
}

void HCombToolbarWidget::initConnect(){
    connectButtons(m_btnStart, m_btnPause);
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
