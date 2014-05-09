#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfiledialog.h>
#include <QToolBar>
#include <QActionGroup>
#include <QProcess>
#include <QMessageBox>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <QTextStream>
#include <QByteArray>
#include <QDataStream>
#include "edittextdialog.h"

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

    QMenu *pEditMenu = ui->menuBar->addMenu(tr("&Edit"));
    pEditMenu->addAction(tr("Edit &texts"), this, SLOT(onEditTexts()));

    QToolBar *mainToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);
    mainToolBar->addAction("New", this, SLOT(onNewMap()));
    mainToolBar->addAction("Open", this, SLOT(onOpenMap()));
    mainToolBar->addAction("Safe", this, SLOT(onSaveAs()));
    mainToolBar->addAction("Play", this, SLOT(onPlay()));

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
    mMap->writeToFile(OT_MINIMAL);
}
void MainWindow::exportAsZip(const QString &filename) {
    QFile toremove(filename);
    toremove.remove();

    QuaZip zip(filename);
    zip.open(QuaZip::mdAdd);
    QuaZipFile mapFile(&zip);
    QuaZipNewInfo info("tmp.xml");
    info.setPermissions(QFile::WriteOwner | QFile::ReadOwner);
    if (!mapFile.open(QIODevice::WriteOnly | QIODevice::Truncate, info)) {
        qWarning("Map zip pack could not open a file");
    }
    if (!mapFile.isOpen()) {
        qWarning("Map zip pack could not open");

    }
    mapFile.write(mMap->writeToString(OT_MINIMAL).toUtf8());
    mapFile.close();
    zip.close();
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
    if (!mapName.endsWith(".xml")) {
        mapName += ".xml";
    }

    safeMap(mapName);
}

void MainWindow::onPlay() {
    exportAsZip(QDir::currentPath() + QDir::separator() + "run/tmp.zip");

    QProcess process;
    process.setWorkingDirectory(QDir::currentPath() + QDir::separator() + "run");
    QString file = QDir::homePath() + QDir::separator() + "Documents/Projects/Mencus/build_linux/bin/Game";
    if (!process.startDetached(file)) {
        QMessageBox::critical(this,
                              tr("Menucs run..."),
                              tr("An error has occured during the starting process of mencus game. Error message %1:").arg(process.errorString()),
                              QMessageBox::Ok);
    }
}

void MainWindow::onEditTexts() {
    EditTextDialog dialog(mMap->getLanguageResources(), this);
    dialog.exec();
}
