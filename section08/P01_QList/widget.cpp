#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("사원관리");

    connect(ui->pbtSave, &QPushButton::pressed, this, &Widget::slot_pbtSave);
    connect(ui->pbtPrint, &QPushButton::pressed, this, &Widget::slot_pbtPrint);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_pbtSave()
{
    int num = ui->leNum->text().toInt();
    QString name = ui->leName->text();
    QString part = ui->lePart->text();

    tEmployee employee;
    employee.num = num;
    employee.name = name;
    employee.part = part;

    m_employeeList.append(employee);

    ui->leNum->clear();
    ui->leName->clear();
    ui->lePart->clear();
}

void Widget::slot_pbtPrint()
{
    ui->textEdit->clear();

    for( qsizetype i = 0 ; i < m_employeeList.size() ; i++ )
    {
        int num = m_employeeList.at(i).num;
        QString name = m_employeeList.at(i).name;
        QString part = m_employeeList.at(i).part;

        QString str;
        str = QString("[사원번호: %1] [성명: %2] [부서: %3]")
                  .arg(num).arg(name, part);

        ui->textEdit->append(str);
    }
}
