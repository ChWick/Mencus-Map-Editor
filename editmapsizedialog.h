#ifndef EDITMAPSIZEDIALOG_H
#define EDITMAPSIZEDIALOG_H

#include <QDialog>
#include "map.h"

namespace Ui {
class EditMapSizeDialog;
}

class EditMapSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMapSizeDialog(MapPtr map, QWidget *parent = 0);
    ~EditMapSizeDialog();

private:
    Ui::EditMapSizeDialog *ui;
    MapPtr mMap;

public slots:
    void accept();
};

#endif // EDITMAPSIZEDIALOG_H
