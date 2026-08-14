#include "widget.h"
#include <QAbstractItemModel>
#include <QListView>
#include <QLabel>
#include <QVBoxLayout>
#include <QStringListModel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Model/View - QListView");

    resize(600, 300);
    QStringList strList;
    strList << "월요일" << "화요일" << "수요일" << "목요일" << "금요일";

    QAbstractItemModel *model = new QStringListModel(strList);

    QListView *view = new QListView();
    view->setModel(model);

    QModelIndex index = model->index(3, 0);
    QString text = model->data(index, Qt::DisplayRole).toString();

    QLabel *lbl = new QLabel(text);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(view);
    lay->addWidget(lbl);

    setLayout(lay);
}

Widget::~Widget() = default;
