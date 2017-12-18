#ifndef HABSTRACTITEM_H
#define HABSTRACTITEM_H

#include <QRect>
#include <string.h>
#define MAXLEN_STR  256

class HAbstractItem
{
public:
    HAbstractItem();
    virtual ~HAbstractItem();

    virtual void clone(HAbstractItem* rhs){
        memcpy(this, rhs, sizeof(HAbstractItem));
    }

    virtual void add();
    virtual void remove();
    virtual void modify();
    virtual void undo();
    virtual void savePreStatus();

    void addOrMod(){
        if (id == -1)
            add();
        else
            modify();
    }

    enum TYPE{
        NONE = 0,

        SCREEN = 1,

        OVERLAY = 10,
        PICTURE,
        TEXT,
        OVERLAY_END = 99,

        ALL = 0xFF,
    };

    enum OPERATE{
        OPERATE_NONE = 0,

        OPERATE_ADD,
        OPERATE_REMOVE,
        OPERATE_MODIFY,
    };

    inline bool isOverlay(){
        return type > OVERLAY && type < OVERLAY_END;
    }

public:
    TYPE type;
    int id;
    QRect rc;

    static OPERATE  s_operate;
    static HAbstractItem* s_itemUndo;
    static void onUndo();
};

class HCombItem : public HAbstractItem
{
public:
    HCombItem();

    void clone(HAbstractItem* rhs){
        memcpy(this, rhs, sizeof(HCombItem));
    }

    virtual void remove();
    virtual void modify();
    virtual void undo();
    virtual void savePreStatus();

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

    void clone(HAbstractItem* rhs){
        memcpy(this, rhs, sizeof(HPictureItem));
    }

    virtual void add();
    virtual void remove();
    virtual void modify();

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

    void clone(HAbstractItem* rhs){
        memcpy(this, rhs, sizeof(HTextItem));
    }

    virtual void add();
    virtual void remove();
    virtual void modify();

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

#endif // HABSTRACTITEM_H
