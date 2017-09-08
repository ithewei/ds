#ifndef HEXPREWIDGET_H
#define HEXPREWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>

#define EXPRE_ICON_WIDTH    128
#define EXPRE_ICON_HEIGHT   128

#define CATEGORY_WIDTH      128
#define CATEGORY_HEIGHT     48

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

protected:
    void initList(QListWidget* list, const char* filename);
    void initUI();
    void initConnect();

public:
    QStackedWidget* m_stack;

    QListWidget* m_listExpre;
    QListWidget* m_listCartoonCN;
    QListWidget* m_listCartoonEN;

    QListWidget* m_listCategory;
};

#endif // HEXPREWIDGET_H
