#ifndef HMAINTOOLBAR_H
#define HMAINTOOLBAR_H

#include "qtheaders.h"
#include <QWebEngineView>
#include "hrcloader.h"

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

class HWebToolbar : public HWidget
{
    Q_OBJECT
public:
    explicit HWebToolbar(QWidget *parent = 0);

    virtual void setGeometry(int x, int y, int w, int h){
        HWidget::setGeometry(x,y,w,h);
        m_webview->setFixedSize(w,h);
    }

    virtual void show();

signals:

public slots:

protected:
    void initUI();
    void initConnect();

public:
    HWebView* m_webview;
    HWebContext* m_webContext;

    int need_reload;
};

#include <QVector>
#define MODEL_WIDGET_WIDTH  110
#define MODEL_WIDGET_HEIGHT 68
#define MODEL_LEFT      10
#define MODEL_TOP       5
#define MODEL_WIDTH     90
#define MODEL_HEIGHT    58

inline void addClassForModelWidget(QPushButton* btn){
    btn->setFixedSize(MODEL_WIDGET_WIDTH,MODEL_WIDGET_HEIGHT);
    btn->setStyleSheet("QPushButton{background-color: #56b9f4;\
                               border-radius: 10px;}"
                  "QPushButton:pressed{background-color: rgb(77, 123, 210)}"
                  "QPushButton:hover{background-color: rgb(77, 123, 210)}");
}

class HModelWidget : public QPushButton{
    Q_OBJECT
public:
    HModelWidget(QWidget* parent = NULL)
        : QPushButton(parent)
    {
        addClassForModelWidget(this);
    }

protected:
    virtual void paintEvent(QPaintEvent* e);

public:
    int w;
    int h;
    QVector<QRect> m_vecRects;
};

#define MAXNUM_MODEL_WIDGET 6
#include "hdsctx.h"
#include "hdsconf.h"
#include "hdsdb.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
class HModelToolbar : public HWidget{
    Q_OBJECT
public:
    HModelToolbar(QWidget* parent = NULL)
        : HWidget(parent)
    {
        initUI();
        initConnect();
    }

    virtual void show(){
        updateUI();
        HWidget::show();
    }

protected:
    void initUI();
    void initConnect();
    void updateUI();

public slots:
    void onBtnHome();
    void onBtnSoundmixer();
    void onBtnTaskinfo();

private:
    HModelWidget* model_widgets;
    QPushButton* btn_home;
    QPushButton* btn_soundmixer;
    QPushButton* btn_taskinfo;
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
