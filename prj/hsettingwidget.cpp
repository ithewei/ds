#include "hsettingwidget.h"
#include "hrcloader.h"

HSettingWidget::HSettingWidget(QWidget* parent)
    : QDialog(parent)
{
    initUI();
    initConnect();
}

#include <QIntValidator>
void HSettingWidget::initUI(){
    setFixedSize(QSize(300,300));
    setStyleSheet("background-color: #FFFFF0");
    setWindowTitle("设置位置");

    QGridLayout* grid = new QGridLayout;

    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(20);

    int row = 0;
    QLabel* label = new QLabel("X:");
    grid->addWidget(label, row, 0);
    m_editX = new QLineEdit;
    QIntValidator* v = new QIntValidator(0, 10000, this);
    m_editX->setValidator(v);
    grid->addWidget(m_editX, row, 1);

    ++row;
    label = new QLabel("Y:");
    grid->addWidget(label, row, 0);
    m_editY = new QLineEdit;
    m_editY->setValidator(v);
    grid->addWidget(m_editY, row, 1);

    ++row;
    label = new QLabel("W:");
    grid->addWidget(label, row, 0);
    m_editW = new QLineEdit;
    m_editW->setValidator(v);
    grid->addWidget(m_editW, row, 1);

    ++row;
    label = new QLabel("H:");
    grid->addWidget(label, row, 0);
    m_editH = new QLineEdit;
    m_editH->setValidator(v);
    grid->addWidget(m_editH, row, 1);

    ++row;
    QHBoxLayout* hbox = new QHBoxLayout;
    //QPushButton* btnAccept = new QPushButton("确认");
    QSize sz(64,64);
    QPushButton* btnAccept = new QPushButton;
    btnAccept->setFixedSize(sz);
    btnAccept->setIconSize(sz);
    btnAccept->setIcon(HRcLoader::instance()->icon_ok);
    btnAccept->setFlat(true);
    QObject::connect( btnAccept, SIGNAL(clicked(bool)), this, SLOT(accept()) );
    hbox->addWidget(btnAccept);

    //QPushButton* btnReject = new QPushButton("取消");
    QPushButton* btnReject = new QPushButton;
    btnReject->setFixedSize(sz);
    btnReject->setIconSize(sz);
    btnReject->setIcon(HRcLoader::instance()->icon_close);
    btnReject->setFlat(true);
    QObject::connect( btnReject, SIGNAL(clicked(bool)), this, SLOT(reject()) );
    hbox->addWidget(btnReject);
    grid->addLayout(hbox, row, 1);

    setLayout(grid);
}

void HSettingWidget::initConnect(){

}

void HSettingWidget::setRect(QRect rc){
    m_rc = rc;
    m_editX->setText( QString::asprintf("%d",rc.x()) );
    m_editY->setText( QString::asprintf("%d",rc.y()) );
    m_editW->setText( QString::asprintf("%d",rc.width()) );
    m_editH->setText( QString::asprintf("%d",rc.height()) );
}

QRect HSettingWidget::getRect(){
    int x = m_editX->text().toInt();
    int y = m_editY->text().toInt();
    int w = m_editW->text().toInt();
    int h = m_editH->text().toInt();
    m_rc.setRect(x,y,w,h);
}
