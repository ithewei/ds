#include "htoolbarwidget.h"

HToolbarWidget::HToolbarWidget(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(105,105,105,204));
    setPalette(pal);
}
