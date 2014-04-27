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

    QMenu *pFileMenu = ui->menuBar->addMenu("File");
    pFileMenu->addAction("Open map", this, SLOT(onOpenMap()));
    pFileMenu->addAction("Exit", this, SLOT(close()));

    QToolBar *mainToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);
    mainToolBar->addAction("Open", this, SLOT(onOpenMap()));

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
