#include "hoperatetarget.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HOperateTargetWidget::HOperateTargetWidget(QWidget* parent)
    : QLabel(parent)
{
       setStyleSheet("border:3px dashed red;");
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

HOperateTarget::HOperateTarget(HAbstractItem* p)
{
    pItem = p;
    wdg = NULL;
}

bool HOperateTarget::isExist(){
    if (pItem->id >= 0)
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

void HOperateTarget::attachWidget(HOperateTargetWidget* p){
    wdg = p;
}

void HOperateTarget::detachWidget(){
    wdg = NULL;
}
