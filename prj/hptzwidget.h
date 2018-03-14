#ifndef HPTZWIDGET_H
#define HPTZWIDGET_H

#include "qtheaders.h"
#include <QToolButton>

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
    QPushButton* m_btnLeft;
    QPushButton* m_btnRight;
    QPushButton* m_btnTop;
    QPushButton* m_btnBottom;
    QPushButton* m_btnNear;
    QPushButton* m_btnFar;
};

#endif // HPTZWIDGET_H
