#ifndef HTITLEBARWIDGET_H
#define HTITLEBARWIDGET_H

#include "qtheaders.h"

class HTitlebarWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HTitlebarWidget(QWidget *parent = 0);

protected:
    void initUI();
    void initConnect();
    virtual bool event(QEvent *e);

public:
    QLabel* m_label;

    QPushButton* m_btnPtz;

    QPushButton* m_btnNum;

    QPushButton* m_btnMicphoneOpened;
    QPushButton* m_btnMicphoneClosed;

    QPushButton* m_btnMute;
    QPushButton* m_btnVoice;

    QPushButton* m_btnStartRecord;
    QPushButton* m_btnStopRecord;

    QPushButton* m_btnSnapshot;

    QPushButton* m_btnFullScreen;
    QPushButton* m_btnExitFullScreen;
};

class HCombTitlebarWidget : public HWidget
{
    Q_OBJECT
public:
    explicit HCombTitlebarWidget(QWidget *parent = 0);

protected:
    void initUI();
    void initConnect();
    virtual bool event(QEvent *e);

public:
    QLabel* m_label;

    QPushButton* m_btnPtz;

    QPushButton* m_btnFullScreen;
    QPushButton* m_btnExitFullScreen;

    QPushButton* m_btnVoice;
    QPushButton* m_btnMute;

    QPushButton* m_btnPinb;
    QPushButton* m_btnPinr;

    QPushButton* m_btnInfob;
    QPushButton* m_btnInfor;

    QPushButton* m_btnSnapshot;

    QPushButton* m_btnStartRecord;
    QPushButton* m_btnStopRecord;
};

#endif // HTITLEBARWIDGET_H
