#include "editmapsizedialog.h"
#include "ui_editmapsizedialog.h"

EditMapSizeDialog::EditMapSizeDialog(MapPtr map, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMapSizeDialog),
    mMap(map)
{
    ui->setupUi(this);

    ui->heightSpinBox->setValue(mMap->getTiles().getSizeY());
    ui->widthSpinBox->setValue(mMap->getTiles().getSizeX());
}

EditMapSizeDialog::~EditMapSizeDialog()
{
    delete ui;
}

void EditMapSizeDialog::accept() {
    QDialog::accept();

    mMap->resize(ui->widthSpinBox->value(), ui->heightSpinBox->value());
}
