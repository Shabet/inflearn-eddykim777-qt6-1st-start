#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(300, 200);

    m_str = QString("버튼");
    m_button = new QPushButton(m_str, this);
    m_button->setGeometry(10, 10, 100, 30);

    connect(m_button, &QPushButton::clicked, this, &Widget::slot_btn);
}

Widget::~Widget() = default;

void Widget::slot_btn()
{
    qDebug() << "Hello World, Button Click.";
}
