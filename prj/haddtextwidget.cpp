#include "haddtextwidget.h"
#include "hrcloader.h"
#include "hdsctx.h"

HAddTextWidget::HAddTextWidget(QWidget *parent) : HWidget(parent){
    initUI();
    initConnect();
}

#include <QIntValidator>
void HAddTextWidget::initUI(){
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255));
    pal.setColor(QPalette::Foreground, QColor(0,0,0));
    setPalette(pal);

    int default_font_size = 48;
    QColor default_font_color = Qt::white;

    QGridLayout* grid = new QGridLayout;
    grid->setSpacing(g_fontsize/2);

    int row = 0;
    grid->addWidget(new QLabel("类别:"), row, 0);

    m_grpCategory = new QButtonGroup(this);
    QRadioButton* btnLabel = new QRadioButton("标签");
    btnLabel->setChecked(true);
    QRadioButton* btnTime = new QRadioButton("时间");
    QRadioButton* btnWatch = new QRadioButton("秒表");
    //QRadioButton* btnSubtitle = new QRadioButton("字幕");
    m_grpCategory->addButton(btnLabel, 0);
    m_grpCategory->addButton(btnTime, 1);
    m_grpCategory->addButton(btnWatch, 2);
    //m_grpCategory->addButton(btnSubtitle, 3);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(btnLabel);
    hbox->addWidget(btnTime);
    hbox->addWidget(btnWatch);
    //hbox->addWidget(btnSubtitle);
    grid->addLayout(hbox, row, 1);

    ++row;
    grid->addWidget(new QLabel(tr("文本:")), row, 0);
    m_editText = new QLineEdit;
    m_editText->setText(tr("请输入标签文本"));
    grid->addWidget(m_editText, row, 1);

    ++row;
    grid->addWidget(new QLabel(tr("字体:")), row, 0);

    hbox = genHBoxLayout();
    hbox->addWidget(new QLabel(tr("字号")));

    m_cmbFontSize = new QComboBox;
    m_cmbFontSize->setFixedWidth(g_fontsize*3);
//    m_cmbFontSize->setEditable(true);
//    QIntValidator* v = new QIntValidator(0, 100, this);
//    m_cmbFontSize->lineEdit()->setValidator(v);
//    m_cmbFontSize->lineEdit()->setAlignment(Qt::AlignCenter);
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
    hbox->addSpacing(g_fontsize*2);
    hbox->addWidget(new QLabel("颜色"));
    m_btnColor = new QPushButton;
    m_btnColor->setFixedWidth(g_fontsize*3);
    m_btnColor->setFlat(true);
    m_btnColor->setStyleSheet("background-color: #FFFFFF; border:2px solid gray;");
    hbox->addWidget(m_btnColor);
    grid->addLayout(hbox, row, 1);

    ++row;
    grid->addWidget(new QLabel("预览:"), row, 0);
    m_labelPreview = new QLabel;
    m_labelPreview->setFixedHeight(100);
    m_labelPreview->setStyleSheet("background-color: #696969");
    pal = m_labelPreview->palette();
    pal.setColor(QPalette::Foreground, default_font_color);
    m_labelPreview->setPalette(pal);
    QFont font = m_labelPreview->font();
    font.setPixelSize(default_font_size);
    m_labelPreview->setFont(font);
    m_labelPreview->setText("123中文ABC");
    grid->addWidget(m_labelPreview, row, 1);

    ++row;
    hbox = genHBoxLayout();
    QPushButton* btnAccept = new QPushButton("确认");
    btnAccept->setDefault(true);
    QObject::connect( btnAccept, SIGNAL(clicked(bool)), this, SLOT(accept()) );
    hbox->addWidget(btnAccept);

    QPushButton* btnReject = new QPushButton("取消");
    QObject::connect( btnReject, SIGNAL(clicked(bool)), this, SLOT(reject()) );
    hbox->addWidget(btnReject);
    grid->addLayout(hbox, row, 1);

    setLayout(grid);

    m_colorSelector = new HColorWidget(this);
    m_colorSelector->setWindowFlags(Qt::Popup | Qt::WindowDoesNotAcceptFocus);
    QObject::connect( m_colorSelector, SIGNAL(newColor(QColor)), this, SLOT(onNewColor(QColor)) );
}

void HAddTextWidget::initConnect(){
    QObject::connect( m_grpCategory, SIGNAL(buttonClicked(int)), this, SLOT(onCategoryChanged(int)));
    QObject::connect( m_btnColor, SIGNAL(clicked(bool)), this, SLOT(selectColor()) );
    QObject::connect( m_cmbFontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontSizeChanged(int)) );
}

void HAddTextWidget::onCategoryChanged(int index){
    if (index == 0){
        m_editText->setText("请输入标签文本");
        m_editText->setReadOnly(false);
    }else if (index == 1){
        m_editText->setText("yyyy-MM-dd HH:mm:ss");
        m_editText->setReadOnly(true);
    }else if (index == 2){
        m_editText->setText("HH:mm:ss:z");
        m_editText->setReadOnly(true);
    }else if (index == 3){
        m_editText->setText("请输入字幕轨");
        m_editText->setReadOnly(false);
    }
}

void HAddTextWidget::selectColor(){
    QPoint ptBotoom = m_btnColor->mapToGlobal(QPoint(0,m_btnColor->height()));
    m_colorSelector->move(ptBotoom.x()-g_fontsize*2, ptBotoom.y());
    m_colorSelector->show();
}

void HAddTextWidget::onNewColor(QColor color){
    if (color.isValid()) {
        m_TextItem.font_color = color.rgb() & 0x00FFFFFF;

        QPalette pal = m_labelPreview->palette();
        pal.setColor(QPalette::Foreground, color);
        m_labelPreview->setPalette(pal);

        m_btnColor->setStyleSheet(QString::asprintf("background-color: #%06x; border:2px solid gray;", m_TextItem.font_color));
    }
}

void HAddTextWidget::onFontSizeChanged(int index){
    QFont font = m_labelPreview->font();
    font.setPixelSize(m_cmbFontSize->currentText().toInt());
    m_labelPreview->setFont(font);
}

void HAddTextWidget::accept(){
    int iCategory = m_grpCategory->checkedId();
    QString text;
    if (iCategory == 0){
        m_TextItem.text_type = HTextItem::LABEL;
        text = m_editText->text();
    }else if (iCategory == 1){
        m_TextItem.text_type = HTextItem::TIME;
        text = "__%%TIMER%%__";
    }else if (iCategory == 2){
        m_TextItem.text_type = HTextItem::WATCHER;
        text = "__%%WATCHER%%__";
    }else if (iCategory == 3){
        m_TextItem.text_type = HTextItem::SUBTITLE;
        text = "__%%subtitle_index%%__";
        text += QString::asprintf("|%d", m_editText->text().toInt());
        qDebug(text.toLocal8Bit().constData());
    }
    strncpy(m_TextItem.text, text.toLocal8Bit().constData(), MAXLEN_STR);
    m_TextItem.font_size = m_cmbFontSize->currentText().toInt();

    hide();
    emit accepted();
    emit newTextItem(m_TextItem);
}

void HAddTextWidget::reject(){
    hide();
    emit rejected();
}
