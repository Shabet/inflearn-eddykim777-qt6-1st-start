#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
    connect(this, &Widget::sig_textChanged, this, &Widget::slot_textChanged);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_valueChanged(int val)
{
    QString str = QString("%1").arg(val);
    ui->leText->setText(str);

    emit sig_textChanged(str);
}

void Widget::slot_textChanged(QString str)
{
    ui->leTextCopy->setText(str);

}
