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

    std::shared_ptr<Map> mMap;

    void exportAsZip(const QString &filename);

signals:
    void sigLoadMap(std::shared_ptr<Map>);
    void sigUpdateMap(MapPtr);

public slots:
    void onSaveAs();
    void onNewMap();
    void onOpenMap();
    void onPlay();
};

#endif // MAINWINDOW_H
