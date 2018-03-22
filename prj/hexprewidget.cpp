#include "hexprewidget.h"
#include "hrcloader.h"
#include "hdsctx.h"
#include "hdsconf.h"

#define CATEGORY_INDEX  Qt::UserRole + 100
#define FILEPATH  Qt::UserRole + 200
#define ACTION    Qt::UserRole + 300

enum EXPRE_ACTION{
    // expre
    SELECT = 1,
    REMOVE = 2,

    // mode
    ADD_MODE = 10,

    REMOVE_MODE = 11,
    SELECT_MODE = 12
};

#include <QDir>
bool delDir(const QString &path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            delDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
HExportWidget::HExportWidget(QWidget* parent)
    :   QDialog(parent)
{
    initUI();
    initConnect();
}

QStringList HExportWidget::selectedFiles(){
    QStringList ret;

    QList<QListWidgetItem*> items = m_listLocal->selectedItems();
    for (int i = 0; i < items.size(); ++i){
        ret.push_back(dsconf->value("PATH/pic_upload") + items[i]->text());
    }

    items = m_listUsb->selectedItems();
    for (int i = 0; i < items.size(); ++i){
        ret.push_back(dsconf->value("PATH/pic_usb") + items[i]->text());
    }

    return ret;
}

void HExportWidget::initUI(){
#if LAYOUT_TYPE_ONLY_OUTPUT
    setFixedSize(QApplication::primaryScreen()->size().width()/2, QApplication::primaryScreen()->size().height()/2);
#endif
    QVBoxLayout* vbox = new QVBoxLayout;

    QHBoxLayout* hbox = genHBoxLayout();

    QVBoxLayout* vbox_local = new QVBoxLayout;
    vbox_local->addWidget(new QLabel(STR("本地：")));
    m_listLocal = new QListWidget;
    vbox_local->addWidget(m_listLocal);
    initList(m_listLocal, dsconf->value("PATH/pic_upload"));
    hbox->addLayout(vbox_local);

    QVBoxLayout* vbox_usb = new QVBoxLayout;
    vbox_usb->addWidget(new QLabel(STR("U盘：")));
    m_listUsb = new QListWidget;
    vbox_usb->addWidget(m_listUsb);
    initList(m_listUsb, dsconf->value("PATH/pic_usb"));
    hbox->addLayout(vbox_usb);

    vbox->addLayout(hbox);

    QHBoxLayout* hbox_okcancel = genHBoxLayout();
    hbox_okcancel->addSpacing(width()/2);
    QPushButton* btnAccept = new QPushButton(STR("确认"));
    //QSize sz(g_fontsize*2,g_fontsize*2);
    //QPushButton* btnAccept = genPushButton(sz, rcloader->get(RC_SUBMIT));
    QObject::connect( btnAccept, SIGNAL(clicked(bool)), this, SLOT(accept()) );
    hbox_okcancel->addWidget(btnAccept);

    QPushButton* btnReject = new QPushButton(STR("取消"));
    //QPushButton* btnReject = genPushButton(sz, rcloader->get(RC_CLOSE));
    QObject::connect( btnReject, SIGNAL(clicked(bool)), this, SLOT(reject()) );
    hbox_okcancel->addWidget(btnReject);
    vbox->addLayout(hbox_okcancel);

    setLayout(vbox);
}

void HExportWidget::initConnect(){

}

void HExportWidget::initList(QListWidget* list, QString dir){
    QDir qdir(dir);
    qdir.setFilter(QDir::Files);
    QStringList filters;
    //filters << "*.png" << "*.jpg" << "*.bmp";
    filters << "*.png";
    qdir.setNameFilters(filters);
    QFileInfoList  files = qdir.entryInfoList();
    for (int i = 0; i < files.size(); ++i){
        QString filepath = files[i].fileName();
        list->addItem(filepath);
    }
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
IMPL_SINGLETON(HExpreWidget)

HExpreWidget::HExpreWidget(QWidget *parent) : HWidget(parent){
    icon_w = icon_h = g_fontsize * 5;
    initUI();
    initConnect();
}

QListWidgetItem* HExpreWidget::genListWidgetItem(QPixmap pixmap){
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(icon_w, icon_h));
    pixmap.scaled(icon_w, icon_h, Qt::KeepAspectRatio);
    item->setIcon(pixmap);
    item->setBackgroundColor(QColor(255,255,0,128));
    return item;
}

QListWidgetItem* HExpreWidget::genListWidgetItem(QString img_file){
    QPixmap pixmap;
    pixmap.load(img_file);
    if (!pixmap.isNull()){
        QListWidgetItem* item =  genListWidgetItem(pixmap);
        item->setData(FILEPATH, img_file);
        item->setData(ACTION, SELECT);
        return item;
    }

    return NULL;
}

#include <QDirIterator>
void HExpreWidget::initList(QListWidget* list, QString filepath){
    list->setMovement(QListView::Static);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(icon_w,icon_h));
    list->setFlow(QListView::LeftToRight);
    list->setSpacing(5);
    list->setFixedWidth(width()-2);

    QListWidgetItem* item = genListWidgetItem(rcloader->get(RC_ADD));
    item->setData(ACTION, ADD_MODE);
    list->addItem(item);

    QDirIterator iter(filepath, QDir::Files);
    bool bHave = false;
    while (iter.hasNext()){
        QString file = iter.next();
        QListWidgetItem* item = genListWidgetItem(file);
        list->addItem(item);
        bHave = true;
    }

    if (bHave){
        QListWidgetItem* item = genListWidgetItem(rcloader->get(RC_SUB));
        item->setData(ACTION, REMOVE_MODE);
        list->addItem(item);
    }

    list->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
                                           "{"
                                           "width:48px;"
                                           "background:rgba(0,0,0,0%);"
                                           "margin:0px,0px,0px,0px;"
                                           "padding-top:48px;"
                                           "padding-bottom:48px;"
                                           "}"
                                           "QScrollBar::handle:vertical"
                                           "{"
                                           "width:48px;"
                                           "background:rgba(0,0,0,25%);"
                                           " border-radius:4px;"
                                           "min-height:48;"
                                           "}"
                                           "QScrollBar::handle:vertical:hover"
                                           "{"
                                           "width:48px;"
                                           "background:rgba(0,0,0,50%);"
                                           " border-radius:4px;"
                                           "min-height:48;"
                                           "}"
                                           "QScrollBar::add-line:vertical"
                                           "{"
                                           "height:48px;width:48px;"
                                           "subcontrol-position:bottom;"
                                           "}"
                                           "QScrollBar::sub-line:vertical"
                                           "{"
                                           "height:48px;width:48px;"
                                           "subcontrol-position:top;"
                                           "}"
                                           "QScrollBar::add-line:vertical:hover"
                                           "{"
                                           "height:48px;width:48px;"
                                           "subcontrol-position:bottom;"
                                           "}"
                                           "QScrollBar::sub-line:vertical:hover"
                                           "{"
                                           "height:48px;width:48px;"
                                           "subcontrol-position:top;"
                                           "}"
                                           "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
                                           "{"
                                           "background:rgba(0,0,0,10%);"
                                           "border-radius:4px;"
                                           "}"
                                           );
}

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
void HExpreWidget::readConf(){
    QString strFile = dsconf->value("PATH/pic_upload");
    strFile += "conf.json";

    FILE* fp = fopen(strFile.toLocal8Bit().constData(), "r");
    if (!fp)
        return;

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* data = (char*)malloc(len+1);
    memset(data,0,len+1);
    fread(data, 1, len, fp);
    fclose(fp);

    QByteArray bytes(data, strlen(data));
    qDebug(bytes.constData());
    QJsonParseError err;
    QJsonDocument dom = QJsonDocument::fromJson(bytes, &err);
    qDebug("err: code=%d offset=%d", err.error, err.offset);
    if (!dom.isNull()){
        if (dom.isObject()){
            QJsonObject obj = dom.object();
            if (obj.contains("maxindex")){
                m_conf.maxindex = obj.value("maxindex").toInt();
            }

            if (obj.contains("record")){
                QJsonValue record = obj.value("record");
                if (record.isArray()){
                    QJsonArray arr = record.toArray();
                    m_conf.records.clear();
                    for (int i = 0; i < arr.size(); ++i){
                        QJsonValue val = arr[i];
                        if (val.isObject()){
                            QJsonObject obj = val.toObject();
                            ExpreRecord record;
                            if (obj.contains("id")){
                                record.id = obj.value("id").toInt();
                            }
                            if (obj.contains("label")){
                                record.label = obj.value("label").toString();
                            }

                            if (obj.contains("dir")){
                                record.dir = obj.value("dir").toString();
                            }

                            m_conf.records.push_back(record);
                        }
                    }
                }
            }
        }
    }

    free(data);
}

void HExpreWidget::writeConf(){
    QJsonArray arr;
    std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
    m_conf.maxindex = 0;
    for (int i = 0; iter != m_conf.records.end(); ++i, ++iter){
        ExpreRecord record = *iter;
        QJsonObject obj;
        obj.insert("id", record.id);
        obj.insert("label", record.label);
        obj.insert("dir", record.dir);
        arr.push_back(obj);

        if (m_conf.maxindex < record.id)
            m_conf.maxindex = record.id;
    }

    QJsonObject obj;
    obj.insert("maxindex", m_conf.maxindex);
    obj.insert("record", arr);

    QJsonDocument dom;
    dom.setObject(obj);

    QByteArray bytes = dom.toJson();

    QString strFile = dsconf->value("PATH/pic_upload");
    strFile += "conf.json";

    FILE* fp = fopen(strFile.toLocal8Bit().constData(), "w");
    if (!fp)
        return;

    fwrite(bytes.data(),1, bytes.length(), fp);
    fclose(fp);
}

void HExpreWidget::genUI(){
    m_listCategory->clear();
    for (int i = m_stack->count()-1; i >=0; --i){
        m_stack->removeWidget(m_stack->widget(i));
    }

    std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
    for (int i = 0; iter != m_conf.records.end(); ++i, ++iter){
        ExpreRecord record = *iter;

        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(QSize(g_fontsize * 5, m_listCategory->height()));
        item->setBackgroundColor(QColor(128,128,128));
        item->setTextColor(QColor(255,255,255));
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(record.label);
        item->setData(CATEGORY_INDEX, i);
        item->setData(FILEPATH, dsconf->value("PATH/pic_upload")+record.dir);
        m_listCategory->addItem(item);

        QListWidget* list = new QListWidget;
        m_stack->addWidget(list);
        initList(list, item->data(FILEPATH).toString());
        QObject::connect( list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onSelectExpre(QListWidgetItem*)) );
    }

    m_listCategory->setCurrentRow(0);
    m_stack->setCurrentIndex(0);
}

void HExpreWidget::initUI(){
#if LAYOUT_TYPE_OUTPUT_AND_INPUT
    setFixedSize((icon_w + 5)*4 + 60, icon_h * 2 + 60);
#else
    setFixedSize((icon_w + 5)*4 + 60, icon_h * 3 + 60);
#endif
    setStyleSheet("background-color: white");

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(1);

    m_stack = new QStackedWidget;

    vbox->addWidget(m_stack);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(1);

    m_listCategory = new QListWidget;
    m_listCategory->setStyleSheet("background-color: white; border: 0px");
    m_listCategory->setMovement(QListView::Static);
    int category_w = g_fontsize * 5;
    int category_h = g_fontsize * 1.5;
    m_listCategory->setMinimumSize(QSize(category_w,category_h));
    m_listCategory->setFixedSize(category_w*4, category_h);
    m_listCategory->setFlow(QListView::LeftToRight);

    hbox->addWidget(m_listCategory);
    hbox->setAlignment(m_listCategory, Qt::AlignLeft);

    hbox->addStretch();

    QSize sz(category_h,category_h);
    m_btnMkdir = genPushButton(sz, rcloader->get(RC_MKDIR));
    m_btnMkdir->hide();
    hbox->addWidget(m_btnMkdir);

    m_btnRmdir = genPushButton(sz, rcloader->get(RC_RMDIR));
    m_btnRmdir->hide();
    hbox->addWidget(m_btnRmdir);

    vbox->addLayout(hbox);

    setLayout(vbox);

    readConf();
    genUI();
}

void HExpreWidget::initConnect(){
    QObject::connect(m_listCategory, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onSelectCategory(QListWidgetItem*)) );
    QObject::connect(m_btnMkdir, SIGNAL(clicked(bool)), this, SLOT(onMkdir()) );
    QObject::connect(m_btnRmdir, SIGNAL(clicked(bool)), this, SLOT(onRmdir()) );
}

void HExpreWidget::onSelectCategory(QListWidgetItem* item){
    int i = item->data(CATEGORY_INDEX).toInt();
    m_stack->setCurrentIndex(i);
    item->setSelected(true);
}

void HExpreWidget::onSelectExpre(QListWidgetItem* item){
    QListWidget* list = item->listWidget();
    QString filepath = m_listCategory->currentItem()->data(FILEPATH).toString();
    qDebug(filepath.toLocal8Bit().constData());

    if (!list || !QDir(filepath).exists())
        return;

    int action = item->data(ACTION).toInt();
    switch (action){
    case SELECT:
    {
        hide();
        QString img_file = item->data(FILEPATH).toString();
        emit expreSelected(img_file);
    }
        break;
    case REMOVE:
    {
        QString img_file = item->data(FILEPATH).toString();
        QFile(img_file).remove();
        list->clear();
        initList(list, filepath);
    }
        break;
    case ADD_MODE:
    {
        HExportWidget dlg(this);
        dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);

        if (dlg.exec() == QDialog::Accepted){
            QStringList files = dlg.selectedFiles();

            for (int i = 0; i < files.size(); ++i){
                QFileInfo file(files.at(i));
                QString newfile(filepath);
                newfile += "/";
                newfile += file.fileName();
                QFile::copy(files.at(i), newfile);
            }

            list->clear();
            initList(list, filepath);
        }
#if LAYOUT_TYPE_OUTPUT_AND_INPUT
        show();
#endif
    }
        break;
    case REMOVE_MODE:
    {
        for (int i = 0; i < list->count(); ++i){
            if (list->item(i)->data(ACTION).toInt() == SELECT){
                list->item(i)->setBackgroundColor(QColor(255,0,0,255));
                list->item(i)->setData(ACTION, REMOVE);
            }
        }
        item->setData(ACTION, SELECT_MODE);
    }
        break;
    case SELECT_MODE:
    {
        for (int i = 0; i < list->count(); ++i){
            if (list->item(i)->data(ACTION).toInt() == REMOVE){
                list->item(i)->setBackgroundColor(QColor(255,255,0,128));
                list->item(i)->setData(ACTION, SELECT);
            }
        }
        item->setData(ACTION, REMOVE_MODE);
    }
        break;
    default:
        break;
    }
}

void HExpreWidget::onMkdir(){
    QString label = QInputDialog::getText(this, STR("新建分类"), STR("请输入一个分类名称："));
    if (label.length() == 0)
        return;

    qDebug(label.toLocal8Bit().constData());

    std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
    for (int i = 0; iter != m_conf.records.end(); ++i, ++iter){
        ExpreRecord record = *iter;
        if (label == record.label){
            QMessageBox::information(this, STR("新建分类"), STR("已存在相同的分类名，新建分类失败！"));
            return;
        }
    }

    int allocid;
    if (m_conf.maxindex == m_conf.records.size()){
        m_conf.maxindex++;
        allocid = m_conf.maxindex;
    }else{
        for (int i = 1; i < m_conf.maxindex; ++i){
            bool b = true;
            std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
            for (;iter != m_conf.records.end();++iter){
                ExpreRecord record = *iter;
                if (record.id == i){
                    b = false;
                    break;
                }
            }
            if (b){
                allocid = i;
                break;
            }
        }
    }

    char dir[16];
    snprintf(dir, 16, "%02d", allocid);

    QDir(dsconf->value("PATH/pic_upload")).mkdir(dir);

    ExpreRecord record;
    record.id = allocid;
    record.label = label;
    record.dir = dir;
    m_conf.records.push_back(record);

    writeConf();
    genUI();
}

void HExpreWidget::onRmdir(){
    QListWidgetItem* item = m_listCategory->currentItem();
    if (!item)
        return;

    QString label = item->text();

    char info[256];
    snprintf(info, 256, "确定删除分类《%s》及其下的所有图片吗？", label.toLocal8Bit().constData());
    if (QMessageBox::question(this, STR("删除分类"), info) ==  QMessageBox::Yes){
        std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
        for (int i = 0; iter != m_conf.records.end(); ++i, ++iter){
            ExpreRecord record = *iter;
            if (label == record.label){
                QString dir = dsconf->value("PATH/pic_upload");
                dir += record.dir;
                delDir(dir);
                m_conf.records.erase(iter);
                break;
            }
        }

        writeConf();
        genUI();
    }
}
