#include "hnumselectwidget.h"
#include "hrcloader.h"

HNumSelectWidget::HNumSelectWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HNumSelectWidget::initUI(){
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(1);

    QSize sz(NUM_ICON_WIDTH,NUM_ICON_HEIGHT);
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        m_numSelects[i] = genPushButton(sz, HRcLoader::instance()->icon_numb[i]);
        hbox->addWidget(m_numSelects[i]);

        m_numCancels[i] = genPushButton(sz, HRcLoader::instance()->icon_numr[i]);
        m_numCancels[i]->hide();
        hbox->addWidget(m_numCancels[i]);

        hbox->addSpacing(10);
    }

    setLayout(hbox);
}

#include <QSignalMapper>
void HNumSelectWidget::initConnect(){
    QSignalMapper* smSelect = new QSignalMapper(this);
    QObject::connect(smSelect, SIGNAL(mapped(int)), this, SIGNAL(numSelected(int)) );

    QSignalMapper* smCancel = new QSignalMapper(this);
    QObject::connect(smCancel, SIGNAL(mapped(int)), this, SIGNAL(numCanceled(int)) );

    for (int i = 0; i < MAX_NUM_ICON; ++i){
        connectButtons(m_numSelects[i], m_numCancels[i]);

        smSelect->setMapping(m_numSelects[i], i+1);
        QObject::connect(m_numSelects[i], SIGNAL(clicked(bool)), smSelect, SLOT(map()) );

        smSelect->setMapping(m_numCancels[i], i+1);
        QObject::connect(m_numCancels[i], SIGNAL(clicked(bool)), smCancel, SLOT(map()) );
    }
}
