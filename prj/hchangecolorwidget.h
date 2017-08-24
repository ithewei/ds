#ifndef HCHANGECOLORWIDGET_H
#define HCHANGECOLORWIDGET_H

#include <QLabel>
#include <QColor>

class HChangeColorWidget : public QLabel
{
public:
    HChangeColorWidget(QWidget* parent = NULL);

    void setOriginColor(QColor color) {m_originColor = color;}
    void setChangeColor(QColor color) {m_changeColor = color;}

    void changeColor(QColor color);

protected:
    virtual void enterEvent(QEvent* e);
    virtual void leaveEvent(QEvent* e);

private:
    QColor m_originColor;
    QColor m_changeColor;
    QColor m_curColor;
};

#endif // HCHANGECOLORWIDGET_H
