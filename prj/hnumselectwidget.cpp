#include "hnumselectwidget.h"
#include "hrcloader.h"

HNumSelectWidget::HNumSelectWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

void HNumSelectWidget::initUI(){
    setFixedSize(260, 66);

    QHBoxLayout* hbox = genHBoxLayout();

    QSize sz(64,64);
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        m_numSelects[i] = genPushButton(sz, rcloader->get(RCID(RC_NUM_B1 + i)));
        hbox->addWidget(m_numSelects[i]);

        m_numCancels[i] = genPushButton(sz, rcloader->get(RCID(RC_NUM_R1 + i)));
        m_numCancels[i]->hide();
        hbox->addWidget(m_numCancels[i]);
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

        smCancel->setMapping(m_numCancels[i], i+1);
        QObject::connect(m_numCancels[i], SIGNAL(clicked(bool)), smCancel, SLOT(map()) );
    }
}
