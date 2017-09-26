#ifndef HABSTRACTITEM_H
#define HABSTRACTITEM_H

#include <QRect>

class HAbstractItem
{
public:
    HAbstractItem();
    virtual ~HAbstractItem();

    virtual void add() = 0;
    virtual void remove() = 0;
    virtual void modify() = 0;

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

    inline bool isOverlay(){
        return type > OVERLAY && type < OVERLAY_END;
    }

public:
    TYPE type;
    int id;
    QRect rc;
};

class HScreenItem : public HAbstractItem
{
public:
    HScreenItem();

    virtual void add();
    virtual void remove();
    virtual void modify();

public:
    int srvid;
    bool v;
    bool a;
    QString src;

    bool bMainScreen;
};

class HPictureItem : public HAbstractItem
{
public:
    HPictureItem();

    virtual void add();
    virtual void remove();
    virtual void modify();

public:
    QString src;
};

class HTextItem : public HAbstractItem
{
public:
    HTextItem();

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
    QString text;
    int font_size;
    int font_color;
};

#endif // HABSTRACTITEM_H
