#include "hexprewidget.h"
#include "hrcloader.h"
#include "hdsctx.h"

#define CATEGORY_INDEX  Qt::UserRole + 100
#define EXPRE_FILEPATH  Qt::UserRole + 200

const char* dir_upload = "/var/www/transcoder/Upload/";

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

HExpreWidget::HExpreWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
}

#include <QBoxLayout>
#include <QDirIterator>
#include <QScrollBar>
void HExpreWidget::initList(QListWidget* list, QString dir){
    list->setMovement(QListView::Static);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(EXPRE_ICON_WIDTH,EXPRE_ICON_HEIGHT));
    list->setMinimumSize(EXPRE_ICON_WIDTH,EXPRE_ICON_HEIGHT);
    list->setFlow(QListView::LeftToRight);
    list->setSpacing(5);
    list->setFixedWidth(EXPRE_WIDTH-2);

    QString filepath = dir_upload;
    filepath += dir;
    QDirIterator iter(filepath, QDir::Files);

    QListWidgetItem* item = new QListWidgetItem;
    item->setIcon(HRcLoader::instance()->icon_add);
    item->setSizeHint(QSize(EXPRE_ICON_WIDTH, EXPRE_ICON_HEIGHT));
    item->setBackgroundColor(QColor(255,255,0,128));
    item->setData(EXPRE_FILEPATH,filepath);
    list->addItem(item);

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

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
void HExpreWidget::readConf(){
    QString strFile = dir_upload;
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

    QString strFile = dir_upload;
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
        item->setSizeHint(QSize(CATEGORY_WIDTH, CATEGORY_HEIGHT));
        item->setBackgroundColor(QColor(128,128,128));
        item->setTextColor(QColor(255,255,255));
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(record.label);
        item->setData(CATEGORY_INDEX, i);
        m_listCategory->addItem(item);

        QListWidget* list = new QListWidget;
        m_stack->addWidget(list);
        initList(list, record.dir);
    }

    m_listCategory->setCurrentRow(0);
    m_stack->setCurrentIndex(0);
}

void HExpreWidget::initUI(){
    setFixedSize(QSize(EXPRE_WIDTH, EXPRE_HEIGHT));
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
    m_listCategory->setMinimumSize(QSize(CATEGORY_WIDTH,CATEGORY_HEIGHT));
    m_listCategory->setFixedSize(CATEGORY_WIDTH*3.5, CATEGORY_HEIGHT);
    m_listCategory->setFlow(QListView::LeftToRight);

    hbox->addWidget(m_listCategory);
    hbox->setAlignment(m_listCategory, Qt::AlignLeft);

    hbox->addStretch();

    m_btnMkdir = new QPushButton;
    m_btnMkdir->setFixedSize(CATEGORY_HEIGHT,CATEGORY_HEIGHT);
    m_btnMkdir->setIcon(QIcon(HRcLoader::instance()->icon_mkdir));
    m_btnMkdir->setIconSize(QSize(CATEGORY_HEIGHT,CATEGORY_HEIGHT));
    m_btnMkdir->setFlat(true);
    hbox->addWidget(m_btnMkdir);

    m_btnRmdir = new QPushButton;
    m_btnRmdir->setFixedSize(CATEGORY_HEIGHT,CATEGORY_HEIGHT);
    m_btnRmdir->setIcon(QIcon(HRcLoader::instance()->icon_rmdir));
    m_btnRmdir->setIconSize(QSize(CATEGORY_HEIGHT,CATEGORY_HEIGHT));
    m_btnRmdir->setFlat(true);
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
    QString filepath = item->data(EXPRE_FILEPATH).toString();
    qDebug(filepath.toLocal8Bit().constData());

    QFileInfo file(filepath);
    if (!file.exists())
        return;

    if (file.isFile()){
        hide();
        emit expreSelected(filepath);
    }else if(file.isDir()){
        onAdd(filepath);
    }
}

#include <QMessageBox>
#include <QInputDialog>
void HExpreWidget::onMkdir(){
    QString label = QInputDialog::getText(this, tr("新建分类"), tr("请输入一个分类名称："));
    if (label.length() == 0)
        return;

    qDebug(label.toLocal8Bit().constData());

    std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
    for (int i = 0; iter != m_conf.records.end(); ++i, ++iter){
        ExpreRecord record = *iter;
        if (label == record.label){
            QMessageBox::information(this, tr("新建分类"), tr("已存在相同的分类名，新建分类失败！"));
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

    QDir(dir_upload).mkdir(dir);

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
    if (QMessageBox::question(this, tr("删除分类"), info) ==  QMessageBox::Yes){
        std::list<ExpreRecord>::iterator iter = m_conf.records.begin();
        for (int i = 0; iter != m_conf.records.end(); ++i, ++iter){
            ExpreRecord record = *iter;
            if (label == record.label){
                QString dir = dir_upload;
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

#include <QFileDialog>
#include <QStandardPaths>
void HExpreWidget::onAdd(QString& str){
    const QFileDialog::Options options = QFileDialog::DontUseNativeDialog;
    QList<QUrl> urls;
         urls << QUrl::fromLocalFile("/var/www/transcoder/Upload")
              << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first());

//    QString selectedFilter;
//    QStringList files = QFileDialog::getOpenFileNames(
//                                NULL, tr("导入图片"),
//                                "/var/www/transcoder/Upload",
//                                tr("Image files(*.png *.jpg *.bmp *.tga)"),
//                                &selectedFilter,
//                                options);

//    for (int i = 0; i < files.size(); ++i){
//        QFileInfo file(files.at(i));
//        QString newfile(str);
//        newfile += "/";
//        newfile += file.fileName();
//        QFile::copy(files.at(i), newfile);
//    }
//    genUI();

    QFileDialog dlg(NULL, tr("导入图片"),"/var/www/transcoder/Upload",tr("Image files(*.png *.jpg *.bmp *.tga)"));
    dlg.setOptions(options);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setSidebarUrls(urls);
    //dlg.showMaximized();
    if (dlg.exec() == QDialog::Accepted){
        QStringList files = dlg.selectedFiles();

        for (int i = 0; i < files.size(); ++i){
            QFileInfo file(files.at(i));
            QString newfile(str);
            newfile += "/";
            newfile += file.fileName();
            QFile::copy(files.at(i), newfile);

            genUI();
        }
    }

    show();
}
