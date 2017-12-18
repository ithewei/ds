#ifndef HOPERATETARGET_H
#define HOPERATETARGET_H

#include "qtheaders.h"
#include "habstractitem.h"

class HOperateWidget : public QLabel
{
    Q_OBJECT
public:
    HOperateWidget(QWidget* parent = NULL);

    void setPixmap(const QPixmap& pixmap);
    void setGeometry(const QRect& rc);
    void setBorderColor(QColor color) {border_color = color;}

    virtual void paintEvent(QPaintEvent *e);

public slots:
    void onTimerUpdate();

public:
    QColor border_color;
    QPixmap src_pixmap;
};

class HOperateObject
{
public:
    HOperateObject(HAbstractItem* p = NULL);

    bool isNull();
    bool isExist();
    bool isModifiable();

    void attachItem(HAbstractItem* p);
    void detachItem();

public:
    QRect rcDraw;
    HAbstractItem* pItem;
};

class HOperateTarget{

public:
    HOperateTarget() {pWdg = NULL;}

public:
    bool isNull();
    bool isOperating();
    void cancel();

public:
    HOperateObject obj;
    HOperateWidget* pWdg;
};

#endif // HOPERATETARGET_H
