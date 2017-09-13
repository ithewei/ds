#ifndef HEXPREWIDGET_H
#define HEXPREWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>

#define EXPRE_ICON_WIDTH    128
#define EXPRE_ICON_HEIGHT   128

#define CATEGORY_WIDTH      128
#define CATEGORY_HEIGHT     48

#define EXPRE_WIDTH         572
#define EXPRE_HEIGHT        408

#include <list>
#include <QFileDialog>
struct ExpreRecord{
    int id;
    QString label;
    QString dir;
};

struct ExpreConf{
    int maxindex;
    std::list<ExpreRecord> records;
};

class HExpreWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HExpreWidget(QWidget *parent = 0);

signals:
    void expreSelected(QString& str);

public slots:
    void onSelectCategory(QListWidgetItem* item);
    void onSelectExpre(QListWidgetItem* item);
    void onMkdir();
    void onRmdir();
    void onAdd(QString& str);

protected:
    void readConf();
    void writeConf();
    void genUI();
    void initList(QListWidget* list, QString dir);

    void initUI();
    void initConnect();

public:
    QStackedWidget* m_stack;
    QListWidget* m_listCategory;
    QPushButton* m_btnMkdir;
    QPushButton* m_btnRmdir;

    ExpreConf m_conf;

    QFileDialog* m_fileDlg;
};

#endif // HEXPREWIDGET_H
