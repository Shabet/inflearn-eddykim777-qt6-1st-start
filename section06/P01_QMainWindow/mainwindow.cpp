#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;

    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);

    QToolBar *fileToolBar;
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);

    QDockWidget *dock = new QDockWidget(tr("Target"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QListWidget *customerList = new QListWidget();
    customerList->addItems(QStringList()
                           << "One" << "Two" << "Three" << "Four" << "Five");

    dock->setWidget(customerList);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    statusBar()->showMessage(tr("Ready"));

    QMdiArea* area = new QMdiArea();
    area->setSizePolicy(QSizePolicy::Expanding,
                        QSizePolicy::Expanding);

    QMdiSubWindow* subWindow1 = new QMdiSubWindow();
    subWindow1->resize(300, 200);

    QPushButton *btn = new QPushButton("Button");
    subWindow1->setWidget(btn);

    QMdiSubWindow* subWindow2 = new QMdiSubWindow();
    subWindow2->resize(300, 200);

    area->addSubWindow(subWindow1);
    area->addSubWindow(subWindow2);

    setCentralWidget(area);
}

MainWindow::~MainWindow() = default;

void MainWindow::newFile()
{

}

void MainWindow::open()
{

}
