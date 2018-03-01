#ifndef HPTZWIDGET_H
#define HPTZWIDGET_H

#include "qtheaders.h"
#include <QToolButton>

#define PTZ_BTN_WIDTH   96
#define PTZ_BTN_HEIGHT  96

#define PTZ_NUMERATOR   1
#define PTZ_DENOMINATOR 20

class HPtzWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HPtzWidget(HWidget *parent = 0);

signals:

public slots:
    void ptzLeft();
    void ptzRight();
    void ptzTop();
    void ptzBottom();
    void ptzNear();
    void ptzFar();

protected:
    void initUI();
    void initConnect();

    virtual void mouseMoveEvent(QMouseEvent *e);

public:
    int srvid;
    QToolButton* m_btnLeft;
    QToolButton* m_btnRight;
    QToolButton* m_btnTop;
    QToolButton* m_btnBottom;
    QPushButton* m_btnNear;
    QPushButton* m_btnFar;
};

#endif // HPTZWIDGET_H
