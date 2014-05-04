#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfiledialog.h>
#include <QToolBar>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    QMenu *pFileMenu = ui->menuBar->addMenu(tr("&File"));
    pFileMenu->addAction(tr("&New map"), this, SLOT(onNewMap()));
    pFileMenu->addAction(tr("&Open map"), this, SLOT(onOpenMap()));
    pFileMenu->addAction(tr("Save &as"), this, SLOT(onSaveAs()));
    pFileMenu->addAction(tr("E&xit"), this, SLOT(close()));

    QToolBar *mainToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);
    mainToolBar->addAction("New", this, SLOT(onNewMap()));
    mainToolBar->addAction("Open", this, SLOT(onOpenMap()));
    mainToolBar->addAction("Safe", this, SLOT(onSaveAs()));

    QToolBar *brushesToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, brushesToolBar);
    QActionGroup *pBrushesActionGroup = new QActionGroup(brushesToolBar);
    QAction *pPlaceTileAction = brushesToolBar->addAction("Place tile", ui->mapView, SLOT(onSelectPlaceTile()));
    pPlaceTileAction->setActionGroup(pBrushesActionGroup);
    pPlaceTileAction->setCheckable(true);
    QAction *pMoveAction = brushesToolBar->addAction("Move", ui->mapView, SLOT(onSelectMove()));
    pMoveAction->setActionGroup(pBrushesActionGroup);
    pMoveAction->setCheckable(true);


    pPlaceTileAction->setChecked(true);



    ui->tilesGraphicsView->init();

    onNewMap();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::safeMap(const QString &filePath) {
    mMap->setFilename(filePath);
    mMap->writeToFile();
}

void MainWindow::onNewMap() {
    mMap = std::shared_ptr<Map>(new Map());

    sigUpdateMap(mMap);
}

void MainWindow::onOpenMap() {
    QString mapName = QFileDialog::getOpenFileName(this, tr("Map map"), "", tr("Maps (*.xml)"));
    mMap = std::shared_ptr<Map>(new Map(mapName));

    sigUpdateMap(mMap);
}

void MainWindow::onSaveAs() {
    QString mapName = QFileDialog::getSaveFileName(this, tr("Map map"), "", tr("Maps (*.xml)"));

    safeMap(mapName);
}
