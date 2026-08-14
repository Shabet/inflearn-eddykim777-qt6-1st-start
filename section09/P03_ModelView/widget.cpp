#include "widget.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDate>
#include <QTableView>
#include <QVBoxLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Model/View - QTableView");
    QStandardItemModel *model = new QStandardItemModel(0, 3);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Subject"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));

    model->setVerticalHeaderItem(0, new QStandardItem("Col 1"));
    model->setVerticalHeaderItem(1, new QStandardItem("Col 2"));

    model->setData(model->index(0, 0), "Monitor");
    model->setData(model->index(0, 1), "LCD 24 inch");
    model->setData(model->index(0, 2), QDate(2021, 10, 4));

    model->setData(model->index(1, 0), "CPU");
    model->setData(model->index(1, 1), "Intel core 2 duo");
    model->setData(model->index(1, 2), QDate(2021, 12, 5));

    QTableView *table = new QTableView();
    table->setModel(model);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(table);

    setLayout(lay);
}

Widget::~Widget() = default;
