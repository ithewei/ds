#ifndef HNUMSELECTWIDGET_H
#define HNUMSELECTWIDGET_H

#include "qtheaders.h"
#include "hrcloader.h"

#define NUM_ICON_WIDTH      64
#define NUM_ICON_HEIGHT     64

class HNumSelectWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HNumSelectWidget(QWidget *parent = 0);

signals:
    void numSelected(int num);
    void numCanceled(int num);

protected:
    void initUI();
    void initConnect();

public:
    QPushButton* m_numSelects[MAX_NUM_ICON];
    QPushButton* m_numCancels[MAX_NUM_ICON];
};

#endif // HNUMSELECTWIDGET_H
