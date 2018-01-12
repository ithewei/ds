#include "habstractitem.h"
#include "hnetwork.h"
#include "hdsctx.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HAbstractItem::OPERATE  HAbstractItem::s_operate = HAbstractItem::OPERATE_NONE;
HAbstractItem* HAbstractItem::s_itemUndo = NULL;

HAbstractItem::HAbstractItem()
{
    type = NONE;
    id = -1;
}

HAbstractItem::~HAbstractItem()
{

}

void HAbstractItem::add(){
    s_operate = OPERATE_ADD;
}

void HAbstractItem::remove(){
    s_operate = OPERATE_REMOVE;
}

void HAbstractItem::modify(){
    s_operate = OPERATE_MODIFY;
}

void HAbstractItem::savePreStatus(){
    if (s_itemUndo){
        delete s_itemUndo;
    }
    s_itemUndo = HItemFactory::createItem(type);
    s_itemUndo->clone(this);
}

void HAbstractItem::undo(){
    if (s_itemUndo){
        if (s_operate == OPERATE_ADD){
            //s_itemUndo->remove();
        }
        else if (s_operate == OPERATE_REMOVE)
            s_itemUndo->add();
        else if (s_operate == OPERATE_MODIFY)
            s_itemUndo->modify();
    }
}

void HAbstractItem::onUndo(){
    if (s_itemUndo)
        s_itemUndo->undo();
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HCombItem::HCombItem()
{
    type = SCREEN;

    srvid = 0;
    v = false;
    a = false;

    bMainScreen = false;
}

void HCombItem::remove(){
    HAbstractItem::remove();

    DsCombInfo si = g_dsCtx->m_tComb;
    if (si.items[id].srvid != 0){
        si.items[id].srvid = 0;
        si.items[id].a = false;
        dsnetwork->postCombInfo(si);
    }
}

void HCombItem::modify(){
    HAbstractItem::modify();

    DsCombInfo si = g_dsCtx->m_tComb;
    si.items[id] = *this;
    dsnetwork->postCombInfo(g_dsCtx->m_tComb);
}

void HCombItem::savePreStatus(){
    HAbstractItem::savePreStatus();

    g_dsCtx->m_tCombUndo = g_dsCtx->m_tComb;
}

void HCombItem::undo(){
    dsnetwork->postCombInfo(g_dsCtx->m_tCombUndo);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HPictureItem::HPictureItem()
{
    type = PICTURE;
    pic_type = IMAGE;
}

void HPictureItem::add(){
    HAbstractItem::add();

    dsnetwork->addPicture(*this);
}

void HPictureItem::remove(){
    HAbstractItem::remove();

    dsnetwork->removePicture(*this);
}

void HPictureItem::modify(){
    HAbstractItem::modify();

    dsnetwork->modifyPicture(*this);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HTextItem::HTextItem()
{
    type = TEXT;

    text_type = LABEL;
    font_size = 32;
    font_color = 0xFFFFFF;
}

void HTextItem::add(){
    HAbstractItem::add();

    dsnetwork->addText(*this);
}

void HTextItem::remove(){
    HAbstractItem::remove();

    dsnetwork->removeText(*this);
}

void HTextItem::modify(){
    HAbstractItem::modify();

    dsnetwork->modifyText(*this);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


HAbstractItem* HItemFactory::createItem(HAbstractItem::TYPE type){
    switch (type){
    case HAbstractItem::SCREEN:
        return new HCombItem;
    case HAbstractItem::PICTURE:
        return new HPictureItem;
    case HAbstractItem::TEXT:
        return new HTextItem;
    }

    return NULL;
}
