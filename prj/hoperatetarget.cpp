#include "hoperatetarget.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HOperateTargetWidget::HOperateTargetWidget(QWidget* parent)
    : QLabel(parent)
{
    setStyleSheet("border:3px dashed red;");
    setMargin(0);
}

void HOperateTargetWidget::setPixmap(const QPixmap& pixmap){
    src_pixmap = pixmap;
    QLabel::setPixmap(src_pixmap.scaled(size()));
}

void HOperateTargetWidget::setGeometry(const QRect& rc){
    if (!src_pixmap.isNull()){
        if (rc.size() != size()){
            QLabel::setPixmap(src_pixmap.scaled(rc.size()));
        }
    }
    QLabel::setGeometry(rc);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

HOperateTarget::HOperateTarget(HAbstractItem* p)
{
    pItem = p;
    wdg = NULL;
}

bool HOperateTarget::isExist(){
    if (pItem && pItem->id >= 0)
        return true;
    return false;
}

bool HOperateTarget::isModifiable(){
    if (pItem->type == HAbstractItem::SCREEN){
        HScreenItem* p = (HScreenItem*)pItem;
        if (p->bMainScreen){
            return false;
        }
    }
    return true;
}

void HOperateTarget::attachItem(HAbstractItem* p){
    detachItem();
    pItem = p;
}

void HOperateTarget::detachItem(){
    if (!isExist()){
        delete pItem;
        pItem = NULL;
    }
}

void HOperateTarget::attachWidget(HOperateTargetWidget* p){
    wdg = p;
    wdg->setStyleSheet("border:3px dashed red;");
}

void HOperateTarget::detachWidget(){
    wdg = NULL;
}
