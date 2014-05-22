#ifndef AUTOGENERATETRANSITIONDIALOG_H
#define AUTOGENERATETRANSITIONDIALOG_H

#include <QDialog>
#include "map.h"

namespace Ui {
class AutoGenerateTransitionDialog;
}

class AutoGenerateTransitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoGenerateTransitionDialog(MapPtr map, QWidget *parent = 0);
    ~AutoGenerateTransitionDialog();

private:
    Ui::AutoGenerateTransitionDialog *ui;
    MapPtr mMap;

    void generate();

public slots:
    void accept();
};

#endif // AUTOGENERATETRANSITIONDIALOG_H
