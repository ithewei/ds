#ifndef HTITLEBARWIDGET_H
#define HTITLEBARWIDGET_H

#include <QWidget>

class HTitlebarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HTitlebarWidget(QWidget *parent = 0);

protected:
    void initUI();
    void initConnection();
    virtual bool event(QEvent *e);

public:
    QLabel* m_label;
    QPushButton* m_btnFullScreen;
    QPushButton* m_btnExitFullScreen;
    QPushButton* m_btnDrawInfo;
    QPushButton* m_btnSnapshot;
    QPushButton* m_btnStartRecord;
    QPushButton* m_btnStopRecord;

    QPushButton* m_btnMicphoneOpened;
    QPushButton* m_btnMicphoneClosed;
    QPushButton* m_btnNum;
};

class HCombTitlebarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HCombTitlebarWidget(QWidget *parent = 0);

protected:
    void initUI();
    void initConnection();
    virtual bool event(QEvent *e);

public:
    QLabel* m_label;
    QPushButton* m_btnFullScreen;
    QPushButton* m_btnExitFullScreen;
    QPushButton* m_btnDrawInfo;
    QPushButton* m_btnSnapshot;
    QPushButton* m_btnStartRecord;
    QPushButton* m_btnStopRecord;
};

#endif // HTITLEBARWIDGET_H
