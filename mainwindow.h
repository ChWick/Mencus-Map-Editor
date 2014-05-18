#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map.h>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void safeMap(const QString &filePath);

    Ui::MainWindow *ui;

    MapPtr mMap;

    void exportAsZip(const QString &filename);

signals:
    void sigLoadMap(MapPtr);
    void sigUpdateMap(MapPtr);

public slots:
    void onSave();
    void onSaveAs();
    void onNewMap();
    void onOpenMap();
    void onPlay();
    void onEditTexts();

private slots:
    void onProcessOutput();
    void onEditExecutablePath();
    void onEditMapSize();
};

#endif // MAINWINDOW_H
