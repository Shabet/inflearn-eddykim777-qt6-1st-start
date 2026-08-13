#include "widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    const QStringList btnStr = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };
    for (const QString &text : btnStr) {
        hboxLayout->addWidget(new QPushButton(text));
    }

    QVBoxLayout *vboxLayout = new QVBoxLayout();
    const QStringList vbtnStr = {"Movie", "Drama", "Animation"};
    for (const QString &text : vbtnStr) {
        vboxLayout->addWidget(new QPushButton(text));
    }

    QGridLayout *gridLayout = new QGridLayout();
    const QStringList gbtnStr = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };
    QList<QPushButton *> gbtn;
    gbtn.reserve(gbtnStr.size());
    for (const QString &text : gbtnStr) {
        gbtn.append(new QPushButton(text));
    }

    gridLayout->addWidget(gbtn[0], 0, 0);
    gridLayout->addWidget(gbtn[1], 0, 1);
    gridLayout->addWidget(gbtn[2], 1, 0, 1, 2);
    gridLayout->addWidget(gbtn[3], 2, 0);
    gridLayout->addWidget(gbtn[4], 2, 1);
    gridLayout->addWidget(gbtn[5], 3, 0, 1, 2);

    QVBoxLayout *defaultLayout = new QVBoxLayout();
    defaultLayout->addLayout(hboxLayout);
    defaultLayout->addLayout(vboxLayout);
    defaultLayout->addLayout(gridLayout);

    setLayout(defaultLayout);
}

Widget::~Widget() = default;
