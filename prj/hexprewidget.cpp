#include "hexprewidget.h"

#define CATEGORY_INDEX  Qt::UserRole + 100
#define EXPRE_FILEPATH  Qt::UserRole + 200

const char* dir_expre = "/var/www/transcoder/Upload/";

HExpreWidget::HExpreWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
}

#include <QBoxLayout>
#include <QDirIterator>
#include <QScrollBar>
void HExpreWidget::initList(QListWidget* list, const char* dir){
    list->setMovement(QListView::Static);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(EXPRE_ICON_WIDTH,EXPRE_ICON_HEIGHT));
    list->setMinimumSize(EXPRE_ICON_WIDTH,EXPRE_ICON_HEIGHT);
    list->setFlow(QListView::LeftToRight);
    list->setSpacing(5);

    QString filepath = dir_expre;
    filepath += dir;
    QDirIterator iter(filepath, QDir::Files);
    while (iter.hasNext()){
        QString file = iter.next();
        QPixmap pixmap;
        pixmap.load(file);
        if (!pixmap.isNull()){
            pixmap.scaled(EXPRE_ICON_WIDTH,EXPRE_ICON_HEIGHT);
            QListWidgetItem* item = new QListWidgetItem;
            item->setIcon(pixmap);
            item->setSizeHint(QSize(EXPRE_ICON_WIDTH, EXPRE_ICON_HEIGHT));
            item->setBackgroundColor(QColor(255,255,0,128));
            item->setData(EXPRE_FILEPATH,file);
            list->addItem(item);
        }
    }

    QObject::connect( list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onSelectExpre(QListWidgetItem*)) );

    list->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
                                           "{"
                                           "width:28px;"
                                           "background:rgba(0,0,0,0%);"
                                           "margin:0px,0px,0px,0px;"
                                           "padding-top:28px;"
                                           "padding-bottom:28px;"
                                           "}"
                                           "QScrollBar::handle:vertical"
                                           "{"
                                           "width:28px;"
                                           "background:rgba(0,0,0,25%);"
                                           " border-radius:4px;"
                                           "min-height:20;"
                                           "}"
                                           "QScrollBar::handle:vertical:hover"
                                           "{"
                                           "width:28px;"
                                           "background:rgba(0,0,0,50%);"
                                           " border-radius:4px;"
                                           "min-height:20;"
                                           "}"
                                           "QScrollBar::add-line:vertical"
                                           "{"
                                           "height:28px;width:28px;"
                                           "subcontrol-position:bottom;"
                                           "}"
                                           "QScrollBar::sub-line:vertical"
                                           "{"
                                           "height:28px;width:28px;"
                                           "subcontrol-position:top;"
                                           "}"
                                           "QScrollBar::add-line:vertical:hover"
                                           "{"
                                           "height:28px;width:28px;"
                                           "subcontrol-position:bottom;"
                                           "}"
                                           "QScrollBar::sub-line:vertical:hover"
                                           "{"
                                           "height:28px;width:28px;"
                                           "subcontrol-position:top;"
                                           "}"
                                           "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
                                           "{"
                                           "background:rgba(0,0,0,10%);"
                                           "border-radius:4px;"
                                           "}"
                                           );
}

void HExpreWidget::initUI(){
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(1);

    m_stack = new QStackedWidget;

    m_listExpre = new QListWidget;
    m_stack->addWidget(m_listExpre);
    initList(m_listExpre, "expression");

    m_listCartoonCN = new QListWidget;
    m_stack->addWidget(m_listCartoonCN);
    initList(m_listCartoonCN, "cartoon_cn");
    m_listCartoonCN->hide();

    m_listCartoonEN = new QListWidget;
    m_stack->addWidget(m_listCartoonEN);
    initList(m_listCartoonEN, "cartoon_en");
    m_listCartoonEN->hide();

    vbox->addWidget(m_stack);

    m_listCategory = new QListWidget;
    m_listCategory->setMovement(QListView::Static);
//    m_listCategory->setViewMode(QListView::IconMode);
//    m_listCategory->setIconSize(QSize(CATEGORY_WIDTH,CATEGORY_HEIGHT));
    m_listCategory->setMinimumSize(QSize(CATEGORY_WIDTH,CATEGORY_HEIGHT));
    m_listCategory->setFixedHeight(CATEGORY_HEIGHT+2);
    m_listCategory->setFlow(QListView::LeftToRight);

    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(CATEGORY_WIDTH, CATEGORY_HEIGHT));
    item->setBackgroundColor(QColor(128,128,128));
    item->setTextColor(QColor(255,255,255));
    item->setTextAlignment(Qt::AlignCenter);
    item->setText("常用表情");
    item->setData(CATEGORY_INDEX, 0);
    m_listCategory->addItem(item);
    item->setSelected(true);

    item = new QListWidgetItem;
    item->setSizeHint(QSize(CATEGORY_WIDTH, CATEGORY_HEIGHT));
    item->setBackgroundColor(QColor(128,128,128));
    item->setTextColor(QColor(255,255,255));
    item->setTextAlignment(Qt::AlignCenter);
    item->setText("卡通中文");
    item->setData(CATEGORY_INDEX, 1);
    m_listCategory->addItem(item);

    item = new QListWidgetItem;
    item->setSizeHint(QSize(CATEGORY_WIDTH, CATEGORY_HEIGHT));
    item->setBackgroundColor(QColor(128,128,128));
    item->setTextColor(QColor(255,255,255));
    item->setTextAlignment(Qt::AlignCenter);
    item->setText("卡通英文");
    item->setData(CATEGORY_INDEX, 2);
    m_listCategory->addItem(item);

    vbox->addWidget(m_listCategory);
    vbox->setAlignment(m_listCategory, Qt::AlignBottom);

    setLayout(vbox);
}

void HExpreWidget::initConnect(){
    QObject::connect(m_listCategory, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onSelectCategory(QListWidgetItem*)) );
}

void HExpreWidget::onSelectCategory(QListWidgetItem* item){
    int i = item->data(CATEGORY_INDEX).toInt();
    m_stack->setCurrentIndex(i);
    item->setSelected(true);
}

void HExpreWidget::onSelectExpre(QListWidgetItem* item){
    QString filepath = item->data(EXPRE_FILEPATH).toString();
    qDebug(filepath.toLocal8Bit().constData());

    hide();

    emit expreSelected(filepath);
}

