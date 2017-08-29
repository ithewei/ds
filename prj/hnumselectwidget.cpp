#include "hnumselectwidget.h"

HNumSelectWidget::HNumSelectWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
}

void HNumSelectWidget::initUI(){
    setFixedSize(MAX_NUM_ICON*(NUM_ICON_WIDTH+10)+2, NUM_ICON_HEIGHT+2);

    QHBoxLayout* hbox = new QHBoxLayout;

    hbox->setMargin(1);

    hbox->addStretch();

    for (int i = 0; i < MAX_NUM_ICON; ++i){
        m_numSelects[i] = new HNumWidget;
        m_numSelects[i]->m_nNum = i+1;
        m_numSelects[i]->setFixedSize(NUM_ICON_WIDTH,NUM_ICON_HEIGHT);
        m_numSelects[i]->setIcon(QIcon(HRcLoader::instance()->icon_numb[i]));
        m_numSelects[i]->setIconSize(QSize(NUM_ICON_WIDTH,NUM_ICON_HEIGHT));
        m_numSelects[i]->setFlat(true);
        hbox->addWidget(m_numSelects[i]);

        m_numCancels[i] = new HNumWidget;
        m_numCancels[i]->m_nNum = i+1;
        m_numCancels[i]->setFixedSize(NUM_ICON_WIDTH,NUM_ICON_HEIGHT);
        m_numCancels[i]->setIcon(QIcon(HRcLoader::instance()->icon_numr[i]));
        m_numCancels[i]->setIconSize(QSize(NUM_ICON_WIDTH,NUM_ICON_HEIGHT));
        m_numCancels[i]->setFlat(true);
        m_numCancels[i]->hide();
        hbox->addWidget(m_numCancels[i]);

        hbox->addSpacing(10);
    }

    hbox->addStretch();

    setLayout(hbox);
}

void HNumSelectWidget::initConnect(){
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        QObject::connect(m_numSelects[i], SIGNAL(clicked(bool)), m_numSelects[i], SLOT(hide()) );
        QObject::connect(m_numSelects[i], SIGNAL(clicked(bool)), m_numCancels[i], SLOT(show()) );
        QObject::connect(m_numSelects[i], SIGNAL(clicked(bool)), this, SLOT(onSelected()) );

        QObject::connect(m_numCancels[i], SIGNAL(clicked(bool)), m_numCancels[i], SLOT(hide()) );
        QObject::connect(m_numCancels[i], SIGNAL(clicked(bool)), m_numSelects[i], SLOT(show()) );
        QObject::connect(m_numCancels[i], SIGNAL(clicked(bool)), this, SLOT(onCanceled()) );
    }
}

void HNumSelectWidget::onSelected(){
    HNumWidget* wdg = (HNumWidget*)sender();
    emit numSelected(wdg->m_nNum);
}

void HNumSelectWidget::onCanceled(){
    HNumWidget* wdg = (HNumWidget*)sender();
    emit numCanceled(wdg->m_nNum);
}
