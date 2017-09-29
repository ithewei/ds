#ifndef HSETTINGWIDGET_H
#define HSETTINGWIDGET_H

#include "qtheaders.h"

class HSettingWidget : public QDialog
{
public:
    HSettingWidget(QWidget* parent = NULL);

    void setRect(QRect rc);
    QRect getRect();

protected:
    void initUI();
    void initConnect();

public:
    QLineEdit* m_editX;
    QLineEdit* m_editY;
    QLineEdit* m_editW;
    QLineEdit* m_editH;

private:
    QRect m_rc;
};

#endif // HSETTINGWIDGET_H
