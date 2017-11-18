#ifndef HEFFECTWIDGET_H
#define HEFFECTWIDGET_H

#include "qtheaders.h"

class HEffectWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HEffectWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void initUI();

public:
    QPushButton* m_btnMosaic;
    QPushButton* m_btnBlur;
};

#endif // HEFFECTWIDGET_H
