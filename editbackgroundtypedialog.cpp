#include "editbackgroundtypedialog.h"
#include "ui_editbackgroundtypedialog.h"

EditBackgroundTypeDialog::EditBackgroundTypeDialog(const QString &initValue, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditBackgroundTypeDialog),
    mBackgroud("initValue")
{
    ui->setupUi(this);
    ui->backgroundComboBox->addItem(tr("Forest"), QString("forest"));
    ui->backgroundComboBox->addItem(tr("Fog"), QString("fog"));
    ui->backgroundComboBox->addItem(tr("Clouds"), QString("clouds"));

    int index = ui->backgroundComboBox->findData(initValue);
    if (index > 0) {
        ui->backgroundComboBox->setCurrentIndex(index);
    }
    else {
        ui->backgroundComboBox->setCurrentIndex(0);
        mBackgroud = qvariant_cast<QString>(ui->backgroundComboBox->currentData());
    }
}

EditBackgroundTypeDialog::~EditBackgroundTypeDialog()
{
    delete ui;
}

void EditBackgroundTypeDialog::on_backgroundComboBox_currentIndexChanged(int index)
{
    mBackgroud = qvariant_cast<QString>(ui->backgroundComboBox->itemData(index));
}
