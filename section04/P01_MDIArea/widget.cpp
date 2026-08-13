#include "widget.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSizePolicy>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(600, 400);

    QMdiArea *area = new QMdiArea();
    area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QMdiSubWindow *subWindow1 = new QMdiSubWindow();
    subWindow1->resize(300,200);

    QPushButton *btn = new QPushButton("버튼");
    subWindow1->setWidget(btn);

    QMdiSubWindow *subWindow2 = new QMdiSubWindow();
    subWindow2->resize(300,200);

    area->addSubWindow(subWindow1);
    area->addSubWindow(subWindow2);

    QHBoxLayout *hLay = new QHBoxLayout();
    hLay->addWidget(area);

    setWindowTitle("MDI Area 예제");
    subWindow1->setWindowTitle("서브 윈도우 1");
    subWindow2->setWindowTitle("서브 윈도우 2");

    setLayout(hLay);
}

Widget::~Widget() = default;
