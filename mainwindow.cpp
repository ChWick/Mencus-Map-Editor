#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfiledialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    QMenu *pFileMenu = ui->menuBar->addMenu("File");
    pFileMenu->addAction("Open map", this, SLOT(onOpenMap()));
    pFileMenu->addAction("Exit", this, SLOT(close()));

    ui->tilesGraphicsView->init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenMap() {
    QString mapName = QFileDialog::getOpenFileName(this, tr("Map map"), "", tr("Maps (*.xml)"));
    mMap = std::shared_ptr<Map>(new Map(mapName));

    sigUpdateMap(mMap);
}
