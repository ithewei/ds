#ifndef HADDTEXTWIDGET_H
#define HADDTEXTWIDGET_H

#include "qtheaders.h"
#include "ds_def.h"
#include "hcolorwidget.h"

class HAddTextWidget : public QDialog
{
    Q_OBJECT
public:
    explicit HAddTextWidget(QWidget *parent = 0);

signals:

public slots:
    void selectFont();
    void selectColor();
    void onNewColor(QColor color);
    void onCategoryChanged(int index);
    void onFontSizeChanged(int index);
    virtual void accept();

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

    HTextItem m_TextItem;
};

#endif // HADDTEXTWIDGET_H
