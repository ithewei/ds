#include "hoperatetarget.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HOperateWidget::HOperateWidget(QWidget* parent)
    : QLabel(parent)
{
    //setStyleSheet("border:3px dashed red;");
    setMargin(0);
    border_color.setRgb(255,0,0);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
    timer->start(100);
}

void HOperateWidget::setPixmap(const QPixmap& pixmap){
    src_pixmap = pixmap;
    QLabel::setPixmap(src_pixmap.scaled(size()));
}

void HOperateWidget::setGeometry(const QRect& rc){
    if (!src_pixmap.isNull()){
        if (rc.size() != size()){
            QLabel::setPixmap(src_pixmap.scaled(rc.size()));
        }
    }
    QLabel::setGeometry(rc);
}

void HOperateWidget::onTimerUpdate(){
    if (isVisible()){
        update();
    }
}

void HOperateWidget::paintEvent(QPaintEvent *e){
    QLabel::paintEvent(e);

    static uint i = 0;
    i += 1;
    QPainter painter(this);
    QPen pen = painter.pen();
    pen.setStyle(Qt::DashLine);
    pen.setColor(border_color);
    pen.setDashOffset(i);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawRect(3,3,width()-6,height()-6);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

HOperateObject::HOperateObject(HAbstractItem* p)
{
    pItem = p;
}

bool HOperateObject::isNull(){
    return pItem == NULL;
}

bool HOperateObject::isExist(){
    if (isNull())
        return false;

    if (pItem && pItem->id >= 0)
        return true;
    return false;
}

bool HOperateObject::isModifiable(){
    if (isNull())
        return false;

    if (pItem->type == HAbstractItem::SCREEN){
        HCombItem* p = (HCombItem*)pItem;
        if (p->bMainScreen){
            return false;
        }
    }
    return true;
}

void HOperateObject::attachItem(HAbstractItem* p){
    detachItem();
    pItem = p;
}

void HOperateObject::detachItem(){
    if (isNull())
        return;

    if (!isExist()){
        delete pItem;
        pItem = NULL;
    }
}
//==========================================================================

bool HOperateTarget::isNull(){
    return obj.isNull();
}

bool HOperateTarget::isOperating(){
    return !obj.isNull() && pWdg->isVisible();
}

void HOperateTarget::cancel(){
    pWdg->hide();
    obj.pItem = NULL;
}
