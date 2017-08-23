#include "hchangecolorwidget.h"

HChangeColorWidget::HChangeColorWidget(QWidget* parent)
    : QLabel(parent)
{
    setOriginColor(Qt::transparent);
    setChangeColor(QColor(255, 0, 0, 128));

    setAutoFillBackground(true);
    changeColor(m_originColor);
}

void HChangeColorWidget::changeColor(QColor color){
    if (m_curColor != color){
        m_curColor = color;
        QPalette pal = palette();
        pal.setColor(QPalette::Background, m_curColor);
        setPalette(pal);
    }
}

void HChangeColorWidget::enterEvent(QEvent* e){
    qDebug("enterEvent");
    changeColor(m_originColor);
}

void HChangeColorWidget::leaveEvent(QEvent* e){
    qDebug("leaveEvent");
    changeColor(m_originColor);
}
