#include "habstractitem.h"
#include "hnetwork.h"
#include "hdsctx.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HAbstractItem::HAbstractItem()
{
    type = NONE;
    id = -1;
}

HAbstractItem::~HAbstractItem()
{

}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HScreenItem::HScreenItem()
{
    type = SCREEN;

    srvid = 0;
    v = false;
    a = false;

    bMainScreen = false;
}

void HScreenItem::add(){

}

void HScreenItem::remove(){
    DsScreenInfo si = g_dsCtx->m_tComb;
    if (si.items[id].srvid != 0){
        si.items[id].srvid = 0;
        si.items[id].a = false;
        HNetwork::instance()->postScreenInfo(si);
    }
}

void HScreenItem::modify(){
    DsScreenInfo si = g_dsCtx->m_tComb;
    si.items[id] = *this;
    HNetwork::instance()->postScreenInfo(g_dsCtx->m_tComb);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HPictureItem::HPictureItem()
{
    type = PICTURE;
}

void HPictureItem::add(){
    HNetwork::instance()->addPicture(*this);
}

void HPictureItem::remove(){
    HNetwork::instance()->removePicture(*this);
}

void HPictureItem::modify(){
    HNetwork::instance()->modifyPicture(*this);
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
    HNetwork::instance()->addText(*this);
}

void HTextItem::remove(){
    HNetwork::instance()->removeText(*this);
}

void HTextItem::modify(){
    HNetwork::instance()->modifyText(*this);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
