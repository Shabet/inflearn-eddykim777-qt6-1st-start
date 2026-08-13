#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidget>
#include <QStatusBar>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void newFile();
    void open();
};
#endif // MAINWINDOW_H
