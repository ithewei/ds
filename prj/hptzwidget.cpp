#include "hptzwidget.h"
#include "hrcloader.h"
#include "hdsctx.h"

HPtzWidget::HPtzWidget(HWidget *parent) : HWidget(parent)
{
    initUI();
    initConnect();
}

void HPtzWidget::initUI(){
#if LAYOUT_TYPE_ONLY_OUTPUT
    int w = g_fontsize * 5;
#else
    int w = g_fontsize * 3;
#endif
    int h = w;
    setFixedSize(w*3, h*2);

    QSize sz(w, h);

//    m_btnTop = new QToolButton;
//    m_btnTop->setIconSize(sz);
//    m_btnTop->setArrowType(Qt::UpArrow);

//    m_btnBottom = new QToolButton;
//    m_btnBottom->setIconSize(sz);
//    m_btnBottom->setArrowType(Qt::DownArrow);

//    m_btnLeft = new QToolButton;
//    m_btnLeft->setIconSize(sz);
//    m_btnLeft->setArrowType(Qt::LeftArrow);

//    m_btnRight = new QToolButton;
//    m_btnRight->setIconSize(sz);
//    m_btnRight->setArrowType(Qt::RightArrow);

    m_btnTop = genPushButton(sz, rcloader->get(RC_UP));
    m_btnBottom = genPushButton(sz, rcloader->get(RC_DOWN));
    m_btnLeft = genPushButton(sz, rcloader->get(RC_LEFT));
    m_btnRight = genPushButton(sz, rcloader->get(RC_RIGHT));

    m_btnNear = genPushButton(sz, rcloader->get(RC_ZOOMIN));
    m_btnFar = genPushButton(sz, rcloader->get(RC_ZOOMOUT));


    QGridLayout *grid = new QGridLayout;
    grid->setMargin(0);
    grid->setSpacing(0);

    int row = 0;
    grid->addWidget(m_btnNear, row, 0);
    grid->addWidget(m_btnTop, row, 1);
    grid->addWidget(m_btnFar, row, 2);

    ++row;
    grid->addWidget(m_btnLeft, row, 0);
    grid->addWidget(m_btnBottom, row, 1);
    grid->addWidget(m_btnRight, row, 2);

    setLayout(grid);
}

void HPtzWidget::initConnect(){
    QObject::connect( m_btnLeft, SIGNAL(clicked(bool)), this, SLOT(ptzLeft()) );
    QObject::connect( m_btnRight, SIGNAL(clicked(bool)), this, SLOT(ptzRight()) );
    QObject::connect( m_btnTop, SIGNAL(clicked(bool)), this, SLOT(ptzTop()) );
    QObject::connect( m_btnBottom, SIGNAL(clicked(bool)), this, SLOT(ptzBottom()) );
    QObject::connect( m_btnNear, SIGNAL(clicked(bool)), this, SLOT(ptzNear()) );
    QObject::connect( m_btnFar, SIGNAL(clicked(bool)), this, SLOT(ptzFar()) );
}

void HPtzWidget::mouseMoveEvent(QMouseEvent *e){
    if (e->buttons() != Qt::NoButton) {
        move( e->globalX() - width()/2, e->globalY() - height()/2);
    }
}

void HPtzWidget::ptzLeft(){
    struct task_PTZ_ctrl_s param;
    param.x[0] += g_dsCtx->m_tInit.ptz_num[0];
    param.x[1] = g_dsCtx->m_tInit.ptz_den[0];
    g_dsCtx->ptzControl(srvid, &param);
}

void HPtzWidget::ptzRight(){
    struct task_PTZ_ctrl_s param;
    param.x[0] -= g_dsCtx->m_tInit.ptz_num[1];
    param.x[1] = g_dsCtx->m_tInit.ptz_den[1];
    g_dsCtx->ptzControl(srvid, &param);
}

void HPtzWidget::ptzTop(){
    struct task_PTZ_ctrl_s param;
    param.y[0] += g_dsCtx->m_tInit.ptz_num[2];
    param.y[1] = g_dsCtx->m_tInit.ptz_den[2];
    g_dsCtx->ptzControl(srvid, &param);
}

void HPtzWidget::ptzBottom(){
    struct task_PTZ_ctrl_s param;
    param.y[0] -= g_dsCtx->m_tInit.ptz_num[3];
    param.y[1] = g_dsCtx->m_tInit.ptz_den[3];
    g_dsCtx->ptzControl(srvid, &param);
}

void HPtzWidget::ptzNear(){
    struct task_PTZ_ctrl_s param;
    param.z[0] += g_dsCtx->m_tInit.ptz_num[4];
    param.z[1] = g_dsCtx->m_tInit.ptz_den[4];
    g_dsCtx->ptzControl(srvid, &param);
}

void HPtzWidget::ptzFar(){
    struct task_PTZ_ctrl_s param;
    param.z[0] -= g_dsCtx->m_tInit.ptz_num[5];
    param.z[1] = g_dsCtx->m_tInit.ptz_den[5];
    g_dsCtx->ptzControl(srvid, &param);
}
