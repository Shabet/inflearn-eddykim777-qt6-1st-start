#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private:
    QPushButton *m_button;
    QString m_str;

private slots:
    void slot_btn();
};
#endif // WIDGET_H
