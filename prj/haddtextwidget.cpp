#include "haddtextwidget.h"

HAddTextWidget::HAddTextWidget(QWidget *parent) : QDialog(parent)
{
    initUI();
    initConnect();
}

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QIntValidator>
#include <QButtonGroup>
#include <QRadioButton>
void HAddTextWidget::initUI(){
    setFixedSize(QSize(480,390));

    int default_font_size = 32;
    QColor default_font_color = Qt::white;

    setWindowTitle("添加文字");

    QGridLayout* grid = new QGridLayout;

    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(10);

    int row = 0;
    QLabel* label = new QLabel("类别:");
    grid->addWidget(label, row, 0);
//    m_cmbCategory = new QComboBox;
//    m_cmbCategory->setEditable(true);
//    m_cmbCategory->lineEdit()->setReadOnly(true);
//    m_cmbCategory->lineEdit()->setAlignment(Qt::AlignCenter);
//    QStringList str;
//    str << "字幕" << "时间" << "秒表";
//    m_cmbCategory->addItems(str);
//    grid->addWidget(m_cmbCategory, row, 1);

    m_grpCategory = new QButtonGroup(this);
    QRadioButton* btnText = new QRadioButton("字幕");
    btnText->setChecked(true);
    QRadioButton* btnTime = new QRadioButton("时间");
    QRadioButton* btnWatch = new QRadioButton("秒表");
    m_grpCategory->addButton(btnText, 0);
    m_grpCategory->addButton(btnTime, 1);
    m_grpCategory->addButton(btnWatch, 2);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(btnText);
    hbox->addWidget(btnTime);
    hbox->addWidget(btnWatch);
    grid->addLayout(hbox, row, 1);

    ++row;
    label = new QLabel("文本:");
    grid->addWidget(label, row, 0);
    m_editText = new QLineEdit;
    m_editText->setText("请添加需要的文本");
    grid->addWidget(m_editText, row, 1);

    ++row;
    label = new QLabel("字体:");
    grid->addWidget(label, row, 0);

    hbox = new QHBoxLayout;
    label = new QLabel("字号");
    hbox->addWidget(label);

    m_cmbFontSize = new QComboBox;
    m_cmbFontSize->setFixedWidth(100);
    m_cmbFontSize->setEditable(true);
    QIntValidator* v = new QIntValidator(0, 100, this);
    m_cmbFontSize->lineEdit()->setValidator(v);
    m_cmbFontSize->lineEdit()->setAlignment(Qt::AlignCenter);
    int num[] = {6,9,12,16,20,24,28,32,36,40,48,56,64,72,84,96,-1};
    int i = 0;
    while (num[i] > 0){
        char szNum[4];
        sprintf(szNum, "%d", num[i]);
        m_cmbFontSize->addItem(szNum);
        if (num[i] == default_font_size){
            m_cmbFontSize->setCurrentIndex(i);
        }
        ++i;
    }
    hbox->addWidget(m_cmbFontSize);
    hbox->addStretch();
    m_btnColor = new QPushButton("颜色选择");
    hbox->addWidget(m_btnColor);
    grid->addLayout(hbox, row, 1);

    ++row;
    label = new QLabel("预览:");
    grid->addWidget(label, row, 0);
    m_labelPreview = new QLabel;
    m_labelPreview->setStyleSheet("border:1px solid red;background-color: black");
    QPalette pal = m_labelPreview->palette();
    pal.setColor(QPalette::Foreground, default_font_color);
    m_labelPreview->setPalette(pal);
    QFont font = m_labelPreview->font();
    font.setPointSize(default_font_size);
    m_labelPreview->setFont(font);
    m_labelPreview->setText("12:34:56\n中文简体");
    grid->addWidget(m_labelPreview, row, 1);

    ++row;
    hbox = new QHBoxLayout;
    QPushButton* btnAccept = new QPushButton("确认");
    QObject::connect( btnAccept, SIGNAL(clicked(bool)), this, SLOT(accept()) );
    hbox->addWidget(btnAccept);

    QPushButton* btnReject = new QPushButton("取消");
    QObject::connect( btnReject, SIGNAL(clicked(bool)), this, SLOT(reject()) );
    hbox->addWidget(btnReject);
    grid->addLayout(hbox, row, 1);

    setLayout(grid);
}

void HAddTextWidget::initConnect(){
    QObject::connect( m_btnColor, SIGNAL(clicked(bool)), this, SLOT(selectColor()) );
    //QObject::connect( m_cmbCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(onCategoryChanged(int)) );
    QObject::connect( m_grpCategory, SIGNAL(buttonClicked(int)), this, SLOT(onCategoryChanged(int)));
    QObject::connect( m_cmbFontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontSizeChanged(int)) );
}

void HAddTextWidget::selectFont(){
    QFontDialog::FontDialogOptions options = QFontDialog::DontUseNativeDialog;
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QApplication::font(), NULL, "Select Font", options);
    if (ok) {
        //...
    }
}

#include <QFileDialog>
void HAddTextWidget::selectColor(){
    QColorDialog::ColorDialogOptions options = QColorDialog::DontUseNativeDialog;
    const QColor color = QColorDialog::getColor(Qt::white, NULL, "Select Color", options);

    if (color.isValid()) {
        m_textItem.font_color = color.rgb();
        qDebug("color=%x", m_textItem.font_color);

        QPalette pal = m_labelPreview->palette();
        pal.setColor(QPalette::Foreground, color);
        m_labelPreview->setPalette(pal);
    }
}

void HAddTextWidget::onCategoryChanged(int index){
    if (index == 0){
        m_editText->setText("请添加需要的文本");
        m_editText->setReadOnly(false);
    }else if (index == 1){
        m_editText->setText("yyyy-MM-dd HH:mm:ss");
        m_editText->setReadOnly(true);
    }else if (index == 2){
        m_editText->setText("HH:mm:ss.zzz");
        m_editText->setReadOnly(true);
    }
}

void HAddTextWidget::onFontSizeChanged(int index){
    QFont font = m_labelPreview->font();
    font.setPointSize(m_cmbFontSize->currentText().toInt());
    m_labelPreview->setFont(font);
}

void HAddTextWidget::accept(){
    //int iCategory = m_cmbCategory->currentIndex();
    int iCategory = m_grpCategory->checkedId();
    if (iCategory == 0){
        m_textItem.type = TextItem::PLAIN_TEXT;
        m_textItem.text = m_editText->text();
    }else if (iCategory == 1){
        m_textItem.type = TextItem::TIME;
        m_textItem.text = "__%%TIMER%%__";
    }else if (iCategory == 2){
        m_textItem.type = TextItem::WATCHER;
        m_textItem.text = "__%%WATCHER%%__";
    }
    m_textItem.font_size = m_cmbFontSize->currentText().toInt();

    QDialog::accept();
}
