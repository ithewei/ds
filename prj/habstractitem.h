#ifndef HABSTRACTITEM_H
#define HABSTRACTITEM_H

#include <QRect>
#include <string.h>
#define MAXLEN_STR  256

#define CLONE_FUNC(classname) \
void clone(HAbstractItem* rhs){ \
    memcpy(this, rhs, sizeof(classname)); \
}

class HAbstractItem
{
public:
    HAbstractItem();
    virtual ~HAbstractItem();

    virtual void clone(HAbstractItem* rhs) = 0;

    virtual void add() {}
    virtual void remove() {}
    virtual void modify() {}
    virtual bool undo() {return false;}

    enum TYPE{
        NONE = 0,

        SCREEN = 1,

        OVERLAY = 10,
        PICTURE,
        TEXT,
        OVERLAY_END = 99,

        ALL = 0xFF,
    };

    inline bool isOverlay(){
        return type > OVERLAY && type < OVERLAY_END;
    }

public:
    TYPE type;
    int id;
    QRect rc;
};

enum OPERATE{
    OPERATE_NONE = 0,

    OPERATE_ADD,
    OPERATE_REMOVE,
    OPERATE_MODIFY,
};

class HCombItem : public HAbstractItem
{
public:
    HCombItem();

    CLONE_FUNC(HCombItem)

    virtual void add();
    virtual void remove();
    virtual void modify();
    virtual bool undo();

public:
    int srvid;
    bool v;
    bool a;
    char src[MAXLEN_STR];

    bool bMainScreen;
};

class HPictureItem : public HAbstractItem
{
public:
    HPictureItem();

    CLONE_FUNC(HPictureItem)

    virtual void add();
    virtual void remove();
    virtual void modify();
    virtual bool undo();

public:
    enum PICTURE_TYPE{
        IMAGE = 1,

        // effect
        MOSAIC = 2,
        BLUR = 3,
    };
    int pic_type;
    char src[MAXLEN_STR];

    int transp;
    int pix; // 单元大小for mosaic
};

class HTextItem : public HAbstractItem
{
public:
    HTextItem();

    CLONE_FUNC(HTextItem)

    virtual void add();
    virtual void remove();
    virtual void modify();
    virtual bool undo();

public:
    enum TEXT_TYPE{
        LABEL = 1,
        TIME = 2,
        WATCHER = 3,
        SUBTITLE = 4,
    };

    TEXT_TYPE text_type;
    char text[MAXLEN_STR];
    int font_size;
    int font_color;
};

class HItemFactory
{
public:
static HAbstractItem* createItem(HAbstractItem::TYPE type);
};

#include <QList>
#include "singleton.h"

#define MAX_UNDO_COUNT  10

class HItemUndo{
    DECLARE_SINGLETON(HItemUndo)
private:
    HItemUndo(){
        undo_list.clear();
    }

    ~HItemUndo(){
        for (int i = 0; i < undo_list.size(); ++i){
            if (undo_list[i]){
                delete undo_list[i];
                undo_list[i] = NULL;
            }
        }
        undo_list.clear();
    }

public:
    void clear(){
        undo_list.clear();
    }

    void save(HAbstractItem* item){
        qInfo("undo_list=%d", undo_list.size());
        if (undo_list.size() > MAX_UNDO_COUNT){
            HAbstractItem* item = undo_list.front();
            if (item){
                delete item;
                item = NULL;
            }
            undo_list.pop_front();
        }
        undo_list.push_back(item);
    }

    int undo(){
        if (undo_list.empty())
            return -1;

        HAbstractItem* item = undo_list.back();
        undo_list.pop_back();
        if (item){
            item->undo();
            delete item;
            item = NULL;
        }

        qInfo("undo_list=%d", undo_list.size());
        return undo_list.size();
    }

    QList<HAbstractItem*> undo_list;
};

#endif // HABSTRACTITEM_H
