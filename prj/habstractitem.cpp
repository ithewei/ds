#include "habstractitem.h"
#include "hnetwork.h"
#include "hdsctx.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
IMPL_SINGLETON(HItemUndo)

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
HCombItem::HCombItem()
{
    type = SCREEN;

    srvid = 0;
    v = false;
    a = false;

    bMainScreen = false;
}

void HCombItem::add(){
    HAbstractItem::add();

    DsCombInfo si;
    si.items[0] = *this;
    for (int i = 0; i < g_dsCtx->m_tComb.itemCnt; ++i){
        si.items[i+1] = g_dsCtx->m_tComb.items[i];
    }
    si.itemCnt = g_dsCtx->m_tComb.itemCnt + 1;
    dsnetwork->postCombInfo(si);
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
    dsnetwork->postCombInfo(si);
}

bool HCombItem::undo(){
    modify();
    return true;
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

bool HPictureItem::undo(){
    OPERATE opr = OPERATE_NONE;
    if (id == -1){
        opr = OPERATE_REMOVE;
        for (int i = 0; i < g_dsCtx->m_pics.itemCnt; ++i){
            if (g_dsCtx->m_pics.items[i].rc == rc &&  strstr(g_dsCtx->m_pics.items[i].src, src)){
                id = g_dsCtx->m_pics.items[i].id;
                break;
            }
        }
    }else{
        for (int i = 0; i < g_dsCtx->m_pics.itemCnt; ++i){
            if (g_dsCtx->m_pics.items[i].id == id){
                if (memcmp(&g_dsCtx->m_pics.items[i], this, sizeof(HPictureItem)) != 0)
                    opr = OPERATE_MODIFY;
                break;
            }
        }

        if (opr != OPERATE_MODIFY)
            opr = OPERATE_ADD;
    }

    if (opr == OPERATE_ADD)
        add();
    else if (opr == OPERATE_REMOVE && id != -1)
        remove();
    else if (opr == OPERATE_MODIFY)
        modify();
    else
        return false;

    return true;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HTextItem::HTextItem()
    : text{0}
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

bool HTextItem::undo(){
    OPERATE opr = OPERATE_NONE;
    if (id == -1){
        opr = OPERATE_REMOVE;
        for (int i = 0; i < g_dsCtx->m_texts.itemCnt; ++i){
            if (g_dsCtx->m_texts.items[i].rc.x() == rc.x() &&
                g_dsCtx->m_texts.items[i].text_type == text_type &&
                strcmp(g_dsCtx->m_texts.items[i].text, text) == 0){
                id = g_dsCtx->m_texts.items[i].id;
                break;
            }
        }
    }else{
        for (int i = 0; i < g_dsCtx->m_texts.itemCnt; ++i){
            if (g_dsCtx->m_texts.items[i].id == id){
                if (memcmp(&g_dsCtx->m_texts.items[i], this, sizeof(HTextItem)) != 0)
                    opr = OPERATE_MODIFY;
                break;
            }
        }

        if (opr != OPERATE_MODIFY)
            opr = OPERATE_ADD;
    }

    if (opr == OPERATE_ADD)
        add();
    else if (opr == OPERATE_REMOVE && id != -1)
        remove();
    else if (opr == OPERATE_MODIFY)
        modify();
    else
        return false;

    return true;
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
