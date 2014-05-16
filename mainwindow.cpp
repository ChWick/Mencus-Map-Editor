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
#include "editexecutabledialog.h"
#include <QSettings>
#include <QScrollBar>
#include "editmapsizedialog.h"

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
    pEditMenu->addAction(tr("Edit exeutable &path"), this, SLOT(onEditExecutablePath()));
    pEditMenu->addAction(tr("Edit map &size"), this, SLOT(onEditMapSize()));
    pEditMenu->addAction(tr("Edit map scale"), ui->mapView, SLOT(onEditMapScale()));

    QToolBar *mainToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);
    mainToolBar->addAction("New", this, SLOT(onNewMap()));
    mainToolBar->addAction("Open", this, SLOT(onOpenMap()));
    mainToolBar->addAction("Safe", this, SLOT(onSaveAs()));
    mainToolBar->addAction("Play", this, SLOT(onPlay()))->setShortcut(QKeySequence(Qt::Key_F5));

    QToolBar *brushesToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, brushesToolBar);
    QActionGroup *pBrushesActionGroup = new QActionGroup(brushesToolBar);
    QAction *pPlaceTileAction = brushesToolBar->addAction("Place tile", ui->mapView, SLOT(onSelectPlaceTile()));
    pPlaceTileAction->setActionGroup(pBrushesActionGroup);
    pPlaceTileAction->setCheckable(true);
    pPlaceTileAction->setShortcut(QKeySequence(Qt::Key_P));
    QAction *pMoveAction = brushesToolBar->addAction("Move", ui->mapView, SLOT(onSelectMove()));
    pMoveAction->setActionGroup(pBrushesActionGroup);
    pMoveAction->setCheckable(true);
    pMoveAction->setShortcut(QKeySequence(Qt::Key_M));
    QAction *pEditAction = brushesToolBar->addAction("Edit", ui->mapView, SLOT(onSelectEdit()));
    pEditAction->setActionGroup(pBrushesActionGroup);
    pEditAction->setCheckable(true);
    pEditAction->setShortcut(QKeySequence(Qt::Key_E));


    pPlaceTileAction->setChecked(true);



    ui->tilesGraphicsView->init();

    QObject::connect(ui->flagBlocksCheckBox, SIGNAL(toggled(bool)), ui->entitiesListWidget, SLOT(onChangeBlocksToggled(bool)));
    QObject::connect(ui->flagDeactivableCheckBox, SIGNAL(toggled(bool)), ui->entitiesListWidget, SLOT(onDeactivateableToggled(bool)));
    QObject::connect(ui->flagTimedCheckBox, SIGNAL(toggled(bool)), ui->entitiesListWidget, SLOT(onTimedToggled(bool)));
    QObject::connect(ui->timeSpinBox, SIGNAL(valueChanged(double)), ui->entitiesListWidget, SLOT(onTimeChanged(double)));

    onNewMap();
}

MainWindow::~MainWindow()
{
    sigUpdateMap(MapPtr());
    mMap.reset();
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
    mMap->writeToZip(zip);
    zip.close();
}

void MainWindow::onNewMap() {
    mMap = MapPtr(new Map());

    sigUpdateMap(mMap);
}

void MainWindow::onOpenMap() {
    QString mapName = QFileDialog::getOpenFileName(this, tr("Map map"), "", tr("Maps (*.xml)"));
    mMap = MapPtr(new Map(mapName));

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
    QSettings settings("CWDevelopment", "MencusMapEditor");
    settings.beginGroup("executable");
    QString execPath = settings.value("executable_path").toString();
    QString workingDir = settings.value("working_dir").toString();
    settings.endGroup();

    QDir dir(QDir::currentPath());
    dir.mkpath("tmp");
    QString zipPath = QDir::currentPath() + "/tmp/" + mMap->getMapName() + ".zip";
    //QString zipPath = QDir::homePath() + "/Documents/Projects/Mencus/level/user/" + mMap->getMapName() + ".zip";
    exportAsZip(zipPath);

    QProcess *process = new QProcess(this);
    QObject::connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessOutput()));
    QObject::connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onProcessOutput()));
    process->setWorkingDirectory(QDir::currentPath() + QDir::separator() + "run");
    process->setWorkingDirectory(workingDir);

    QFile backup(process->workingDirectory() + "/backup.xml");
    backup.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly);
    backup.write(QString(
                     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                     "<snapshot version=\"1\" game_state=\"5\" map_name=\"" + mMap->getMapName() + "\"/>").toUtf8());
    backup.close();
    process->start(execPath, QStringList() << QDir::currentPath() + "/tmp");
    process->waitForStarted();
    if (process->state() == QProcess::NotRunning) {
        QMessageBox::critical(this,
                              tr("Menucs run..."),
                              tr("An error has occured during the starting process of mencus game. Error message %1:").arg(process->errorString()),
                              QMessageBox::Ok);
    }
}

void MainWindow::onEditTexts() {
    EditTextDialog dialog(mMap->getLanguageResources(), this);
    dialog.exec();
}

void MainWindow::onProcessOutput() {
    QProcess *process = dynamic_cast<QProcess*>(sender());
    QString out = process->readAllStandardOutput();
    if (out.size() == 0){
        out = process->readAllStandardError();
    }
    if (out.size() != 0) {
        ui->outputText->setPlainText(ui->outputText->toPlainText() + "\n" + out);
        ui->outputText->verticalScrollBar()->setValue(ui->outputText->verticalScrollBar()->maximum());
    }
}

void MainWindow::onEditExecutablePath() {
    EditExecutableDialog dialog(this);
    dialog.exec();
}

void MainWindow::onEditMapSize() {
    EditMapSizeDialog dialog(mMap, this);
    dialog.exec();
    if (dialog.result() == QDialog::Accepted) {
        sigUpdateMap(mMap);
    }
}
