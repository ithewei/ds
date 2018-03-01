#include "htoolbarwidget.h"
#include "hrcloader.h"
#include "hdsctx.h"

HToolbarWidget::HToolbarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();

    m_bCanSlider = true;
}

void HToolbarWidget::initUI(){
    setBgFg(this, MASK_BG);

    QHBoxLayout* hbox = genHBoxLayout();
    hbox->setSpacing(g_dsCtx->m_tInit.spacing);

    QSize sz(g_dsCtx->m_tInit.toolbar_height, g_dsCtx->m_tInit.toolbar_height);

    m_btnStart = genPushButton(sz, rcloader->get(RC_START));
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);

    m_btnPause = genPushButton(sz, rcloader->get(RC_PAUSE));
    hbox->addWidget(m_btnPause);

    m_slider = new QSlider;
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setRange(0,100);
    m_slider->setFixedHeight(32);
    m_slider->setStyleSheet("\
                QSlider::groove:horizontal {\
                border: 2px solid #4A708B;\
                background: #C0C0C0;\
                height: 16px;\
                border-radius: 2px;\
                padding-left:-1px;\
                padding-right:-1px;\
                }\
                \
                QSlider::sub-page:horizontal {\
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \
                    stop:0 #B1B1B1, stop:1 #c4c4c4);\
                background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,\
                    stop: 0 #5DCCFF, stop: 1 #1874CD);\
                border: 2px solid #4A708B;\
                height: 16px;\
                border-radius: 2px;\
                }\
                \
                QSlider::add-page:horizontal {\
                background: #575757;\
                border: 2px solid #777;\
                height: 16px;\
                border-radius: 2px;\
                }\
                QSlider::handle:horizontal \
                {\
                    background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, \
                    stop:0.6 #45ADED, stop:0.778409 rgba(255, 255, 255, 255));\
                    width: 26px;\
                    margin-top: -5px;\
                    margin-bottom: -5px;\
                    border-radius: 13px;\
               }\
                \
               QSlider::handle:horizontal:hover {\
                    background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.6 #2A8BDA, \
                    stop:0.778409 rgba(255, 255, 255, 255));\
                    width: 26px;\
                    margin-top: -5px;\
                    margin-bottom: -5px;\
                    border-radius: 13px;\
                }"
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

        if (action == QAbstractSlider::SliderPageStepAdd){
            last_slider = m_slider->value() + 10;
            last_slider = last_slider > 100 ? 100 : last_slider;
        }else if (action == QAbstractSlider::SliderPageStepSub){
            last_slider = m_slider->value() - 10;
            last_slider = last_slider < 0 ? 0 : last_slider;
        }
        if (m_bCanSlider){
            m_bCanSlider = false;
            QTimer::singleShot(1000, this, SLOT(onTimerSlider()));
        }
    }
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HCombToolbarWidget::HCombToolbarWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HCombToolbarWidget::initUI(){
    setBgFg(this, MASK_BG);

    QHBoxLayout* hbox = genHBoxLayout();
    hbox->setSpacing(g_dsCtx->m_tInit.spacing);

    QSize sz(g_dsCtx->m_tInit.output_toolbar_height, g_dsCtx->m_tInit.output_toolbar_height);

    m_btnStart = genPushButton(sz, rcloader->get(RC_START));
    m_btnStart->hide();
    hbox->addWidget(m_btnStart);

    m_btnPause = genPushButton(sz, rcloader->get(RC_PAUSE));
    hbox->addWidget(m_btnPause);

    hbox->addSpacing(100);

    m_btnUndo = genPushButton(sz, rcloader->get(RC_UNDO));
    hbox->addWidget(m_btnUndo);

    m_btnOK = genPushButton(sz, rcloader->get(RC_OK));
    hbox->addWidget(m_btnOK);

    m_btnTrash = genPushButton(sz, rcloader->get(RC_TRASH));
    hbox->addWidget(m_btnTrash);

    hbox->addStretch();

    m_btnText = genPushButton(sz, rcloader->get(RC_TEXT));
    hbox->addWidget(m_btnText);

    m_btnExpre = genPushButton(sz, rcloader->get(RC_EXPRE));
    hbox->addWidget(m_btnExpre);

    m_btnEffect = genPushButton(sz, rcloader->get(RC_EFFECT));
    m_btnEffect->hide();
    hbox->addWidget(m_btnEffect);

    m_btnZoomOut = genPushButton(sz, rcloader->get(RC_ZOOMOUT));
    hbox->addWidget(m_btnZoomOut);

    m_btnZoomIn = genPushButton(sz, rcloader->get(RC_ZOOMIN));
    hbox->addWidget(m_btnZoomIn);

    m_btnSetting = genPushButton(sz, rcloader->get(RC_SETTING));
    m_btnSetting->hide();
    hbox->addWidget(m_btnSetting);

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
