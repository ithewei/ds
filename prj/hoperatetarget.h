#ifndef HOPERATETARGET_H
#define HOPERATETARGET_H

#include <QObject>
#include <QLabel>
#include "habstractitem.h"

class HOperateTargetWidget : public QLabel
{
    Q_OBJECT
public:
    HOperateTargetWidget(QWidget* parent = NULL);

public:
    QPixmap src_pixmap;
};

class HOperateTarget
{
public:
    HOperateTarget(HAbstractItem* p);

    bool isModifiable();
    bool isExist();

    void attachWidget(HOperateTargetWidget* p);
    void detachWidget();

public:
    QRect rcDraw;
    HAbstractItem* pItem;
    HOperateTargetWidget* wdg;
};

#endif // HOPERATETARGET_H
