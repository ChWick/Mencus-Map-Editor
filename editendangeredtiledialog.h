#ifndef EDITENDANGEREDTILEDIALOG_H
#define EDITENDANGEREDTILEDIALOG_H

#include <QDialog>
#include "map.h"

namespace Ui {
class EditEndangeredTileDialog;
}

class EditEndangeredTileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditEndangeredTileDialog(EndangeredTile &tile, QWidget *parent = 0);
    ~EditEndangeredTileDialog();

private:
    Ui::EditEndangeredTileDialog *ui;
    EndangeredTile &mTile;

public slots:
    void accept();
private slots:
    void on_tileButton_clicked();
};

#endif // EDITENDANGEREDTILEDIALOG_H
