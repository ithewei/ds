#ifndef HEFFECTWIDGET_H
#define HEFFECTWIDGET_H

#include "qtheaders.h"
#include "habstractitem.h"

class HEffectWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HEffectWidget(QWidget *parent = 0);

signals:
    void effectSelected(HPictureItem item);

public slots:
    void onMosaic();
    void onBlur();


protected:
    void initUI();
    void initConnect();

public:
    QPushButton* m_btnMosaic;
    QPushButton* m_btnBlur;
};

#endif // HEFFECTWIDGET_H