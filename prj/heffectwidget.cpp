#include "heffectwidget.h"
#include "hrcloader.h"

HEffectWidget::HEffectWidget(QWidget *parent) : HWidget(parent){
    initUI();
}

void HEffectWidget::initUI(){
    setFixedSize(230, 98);

    QHBoxLayout* hbox = genHBoxLayout();

    QSize sz(96,96);

    m_btnMosaic = genPushButton(sz, HRcLoader::instance()->icon_mosaic);
    hbox->addWidget(m_btnMosaic);

    m_btnBlur = genPushButton(sz, HRcLoader::instance()->icon_blur);
    hbox->addWidget(m_btnBlur);

    setLayout(hbox);
}
