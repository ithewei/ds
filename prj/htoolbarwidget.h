#ifndef HTOOLBARWIDGET_H
#define HTOOLBARWIDGET_H

#include "qtheaders.h"

class HToolbarWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HToolbarWidget(QWidget *parent = 0);
    void setProgress(int value) {
        if (m_bCanSlider){
            m_slider->setValue(value);
        }
    }

signals:
    void progressChanged(int progress);

public slots:
    void onSlider();
    void onSlider(int action);
    void onTimerSlider() {
        emit progressChanged(last_slider);
        m_bCanSlider = true;
    }

protected:
    void initUI();
    void initConnect();
    virtual bool event(QEvent *e);

public:
    QPushButton* m_btnStart;
    QPushButton* m_btnPause;
    QPushButton* m_btnStop;
    QSlider* m_slider;
    bool m_bCanSlider;
    int last_slider;
};

class HCombToolbarWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HCombToolbarWidget(QWidget *parent = 0);

protected:
    void initUI();
    void initConnect();
    virtual bool event(QEvent *e);

public:
    QPushButton* m_btnStart;
    QPushButton* m_btnPause;
    QPushButton* m_btnStop;

    QPushButton* m_btnText;
    QPushButton* m_btnExpre;
    QPushButton* m_btnEffect;

    QPushButton* m_btnZoomIn;
    QPushButton* m_btnZoomOut;

    QPushButton* m_btnSetting;
    QPushButton* m_btnOK;
    QPushButton* m_btnTrash;
    QPushButton* m_btnUndo;
};

#endif // HTOOLBARWIDGET_H
