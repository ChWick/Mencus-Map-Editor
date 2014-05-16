#include "editmapscaledialog.h"
#include "ui_editmapscaledialog.h"

EditMapScaleDialog::EditMapScaleDialog(double initScale, double gridScale, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMapScaleDialog),
    mGridScale(gridScale)
{
    ui->setupUi(this);

    ui->scaleSpinBox->setValue(initScale);
    ui->tileSizeSpinBox->setValue(initScale * 64);
    ui->gridFactorSpinBox->setValue(gridScale);
}

EditMapScaleDialog::~EditMapScaleDialog()
{
    delete ui;
}

void EditMapScaleDialog::on_tileSizeSpinBox_valueChanged(int arg1)
{
    //ui->scaleSpinBox->setValue(static_cast<double>(arg1) / 64);
    mTileSize = arg1;
}


void EditMapScaleDialog::on_scaleSpinBox_valueChanged(double arg1)
{
    ui->tileSizeSpinBox->setValue(arg1 * 64);
    mScale = arg1;
}

void EditMapScaleDialog::on_gridFactorSpinBox_editingFinished()
{
    mGridScale = ui->gridFactorSpinBox->value();
}
