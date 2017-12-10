#ifndef HMAINTOOLBAR_H
#define HMAINTOOLBAR_H

#include "qtheaders.h"
#include <QWebEngineView>

#define ICON_WIDTH      90
#define ICON_HEIGHT     90
#define MAIN_TOOBAR_HEIGHT  92

class HWebContext : public QObject{
    Q_OBJECT
public:
    HWebContext();

signals:
    void sendSelectInfo(const QString &info, int id);

public slots:
    void setAction(int action);
    void toogleInfo();
    void getSelectInfo(int id);
};

class HWebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit HWebView(QWidget* parent = 0);

signals:

public slots:
    void onUrlChanged(QUrl url);

protected:
    virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

private:
    bool m_bAdjustPos;
};

class HMainToolbar : public HWidget
{
    Q_OBJECT
public:
    explicit HMainToolbar(QWidget *parent = 0);

    virtual void show();

signals:

public slots:

protected:
    void initUI();
    void initConnect();

public:
    HWebView* m_webview;
    HWebContext* m_webContext;
};

#include <QSignalMapper>
class HStyleToolbar : public HWidget
{
    Q_OBJECT

    enum EStyle{
        STYLE_1 = 1,
        STYLE_2 = 2,
        STYLE_4 = 4,
        STYLE_9 = 9,
        STYLE_16 = 16,
        STYLE_25 = 25,
        STYLE_36 = 36,
        STYLE_49 = 49,
        STYLE_64 = 64,
    };

public:
    explicit HStyleToolbar(QWidget *parent = 0);

signals:
    void styleChanged(int row, int col);

public slots:
    void onStyleBtnClicked(int style);
    void onReturn();

protected:
    void initUI();
    void initConnect();
    virtual void leaveEvent(QEvent* e);

public:
    QSignalMapper* m_smStyle;
    QPushButton* m_btnStyle1;
    QPushButton* m_btnStyle2;
    QPushButton* m_btnStyle4;
    QPushButton* m_btnStyle9;
    QPushButton* m_btnStyle16;
    QPushButton* m_btnStyle25;
    QPushButton* m_btnStyle36;
    QPushButton* m_btnStyle49;
    QPushButton* m_btnStyle64;

    QPushButton* m_btnMerge;
    QPushButton* m_btnReturn;
};

#endif // HMAINTOOLBAR_H
