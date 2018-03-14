#ifndef QTHEADERS_H
#define QTHEADERS_H

#include <QPoint>
#include <QSize>
#include <QRect>
#include <QTimer>

#include <QApplication>

#include <QWidget>
#include <QScreen>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QFrame>
#include <QComboBox>
#include <QListWidget>
#include <QSlider>
#include <QScrollBar>
#include <QStackedWidget>

#include <QDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QInputDialog>

#include <QLayout>
#include <QBoxLayout>
#include <QGridLayout>

#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>

#include <QButtonGroup>

#include <QPainter>
#include <qdrawutil.h>

//---define some widgets for unified style---------------------------------------------
inline QPushButton* genPushButton(QSize sz, QPixmap pixmap, QWidget* parent = NULL){
    QPushButton* btn = new QPushButton(parent);
    btn->setFlat(true);
    btn->setFixedSize(sz);
    btn->setIconSize(sz);
    btn->setIcon(pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    return btn;
}

inline QHBoxLayout* genHBoxLayout(){
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setContentsMargins(5,1,5,1);
    hbox->setSpacing(QApplication::primaryScreen()->size().width()/80);
    return hbox;
}

#if LAYOUT_LAYOUT_TYPE_ONLY_OUTPUT
#define MASK_BG QColor(0x80, 0x80, 0x80, 0x40)
#else
#define MASK_BG QColor(0x40, 0x40, 0x40, 0xC0)
#endif
inline void setBgFg(QWidget* wdg, QColor bg, QColor fg = Qt::white){
    wdg->setAutoFillBackground(true);
    QPalette pal = wdg->palette();
    pal.setColor(QPalette::Background, bg);
    pal.setColor(QPalette::Foreground, fg);
    wdg->setPalette(pal);
}

inline void connectButtons(QPushButton* btn1, QPushButton* btn2){
    QObject::connect(btn1, SIGNAL(clicked(bool)), btn1, SLOT(hide()) );
    QObject::connect(btn1, SIGNAL(clicked(bool)), btn2, SLOT(show()) );

    QObject::connect(btn2, SIGNAL(clicked(bool)), btn2, SLOT(hide()) );
    QObject::connect(btn2, SIGNAL(clicked(bool)), btn1, SLOT(show()) );
}

inline void centerWidget(QWidget* wdg){
    int w = wdg->width();
    int h = wdg->height();
    int sw = QApplication::desktop()->screenGeometry(0).width();
    int sh = QApplication::desktop()->screenGeometry(0).height();
    if (w < sw && h < sh){
        wdg->setGeometry((sw-w)/2, (sh-h)/2, w, h);
    }
}

inline QRect adjustPos(QRect rc, QRect rcParent){
    int x = qMax(rc.left(), rcParent.left());
    int y = qMax(rc.top(), rcParent.top());
    int w = qMin(rc.width(), rcParent.width());
    int h = qMin(rc.height(), rcParent.height());

    if (rc.right() >= rcParent.right())
        x = rcParent.right() - w;
    if (rc.bottom() >= rcParent.bottom())
        y = rcParent.bottom() - h;

    return QRect(x,y,w,h);
}

typedef QWidget HWidget;
//============================================================================
#endif // QTHEADERS_H
