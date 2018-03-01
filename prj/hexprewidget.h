#ifndef HEXPREWIDGET_H
#define HEXPREWIDGET_H

#include "qtheaders.h"

#include <list>
struct ExpreRecord{
    int id;
    QString label;
    QString dir;
};

struct ExpreConf{
    int maxindex;
    std::list<ExpreRecord> records;
};

class HExportWidget : public QDialog
{
    Q_OBJECT
public:
    HExportWidget(QWidget *parent = NULL);

    QStringList selectedFiles();

protected:
    void initUI();
    void initConnect();

    void initList(QListWidget* list, QString dir);

public:
    QListWidget* m_listLocal;
    QListWidget* m_listUsb;
};

#include <QListWidgetItem>
#include "singleton.h"
class HExpreWidget : public HWidget
{
    Q_OBJECT
    DECLARE_SINGLETON(HExpreWidget)
public:
    explicit HExpreWidget(QWidget *parent = 0);

signals:
    void expreSelected(QString& str);

public slots:
    void onSelectCategory(QListWidgetItem* item);
    void onSelectExpre(QListWidgetItem* item);
    void onMkdir();
    void onRmdir();

protected:
    void readConf();
    void writeConf();
    void genUI();
    QListWidgetItem* genListWidgetItem(QPixmap pixmap);
    QListWidgetItem* genListWidgetItem(QString img_file);
    void initList(QListWidget* list, QString dir);

    void initUI();
    void initConnect();

public:
    QStackedWidget* m_stack;
    QListWidget* m_listCategory;
    QPushButton* m_btnMkdir;
    QPushButton* m_btnRmdir;

public:
    ExpreConf m_conf;
    int icon_w;
    int icon_h;
};

#endif // HEXPREWIDGET_H
