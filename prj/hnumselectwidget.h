#ifndef HNUMSELECTWIDGET_H
#define HNUMSELECTWIDGET_H

#include "qtheaders.h"
#include "hrcloader.h"

#define NUM_ICON_WIDTH      64
#define NUM_ICON_HEIGHT     64

class HNumWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit HNumWidget(QWidget *parent = 0) : QPushButton(parent) {}

public:
    int m_nNum;
};

class HNumSelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HNumSelectWidget(QWidget *parent = 0);

signals:
    void numSelected(int num);
    void numCanceled(int num);

public slots:
    void onSelected();
    void onCanceled();

protected:
    void initUI();
    void initConnect();

public:
    HNumWidget* m_numSelects[MAX_NUM_ICON];
    HNumWidget* m_numCancels[MAX_NUM_ICON];
};

#endif // HNUMSELECTWIDGET_H
