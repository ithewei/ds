#ifndef HTOOLBARWIDGET_H
#define HTOOLBARWIDGET_H

#include <QWidget>
#include <QSlider>

class HToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HToolbarWidget(QWidget *parent = 0);

signals:
    void sigStart();
    void sigPause();
    void progressChanged(int progress);

public slots:
    void onStart();
    void onPause();
    void onSlider();

protected:
    void initUI();
    void initConnection();
    virtual bool event(QEvent *e);

public:
    QPushButton* m_btnStart;
    QPushButton* m_btnPause;
    QSlider* m_slider;
};

#endif // HTOOLBARWIDGET_H
