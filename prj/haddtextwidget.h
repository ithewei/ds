#ifndef HADDTEXTWIDGET_H
#define HADDTEXTWIDGET_H

#include "qtheaders.h"
#include "ds_def.h"
#include "hcolorwidget.h"

class HAddTextWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HAddTextWidget(QWidget *parent = 0);

signals:
    void accepted();
    void rejected();
    void newTextItem(HTextItem);

public slots:
    void selectFont();
    void selectColor();
    void onNewColor(QColor color);
    void onCategoryChanged(int index);
    void onFontSizeChanged(int index);
    void accept();
    void reject();

protected:
    void initUI();
    void initConnect();

public:
    QButtonGroup* m_grpCategory;
    //QComboBox* m_cmbCategory;
    QLineEdit* m_editText;
    QComboBox* m_cmbFontSize;
    QPushButton* m_btnColor;
    QLabel* m_labelPreview;

    HColorWidget* m_colorSelector;

    HTextItem m_TextItem;
};

#endif // HADDTEXTWIDGET_H
