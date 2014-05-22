#include "mappropertiesdialog.h"
#include "ui_mappropertiesdialog.h"

MapPropertiesDialog::MapPropertiesDialog(const QString &dif, bool tut, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapPropertiesDialog),
    mDifficulty(dif),
    mTutorial(tut)
{
    ui->setupUi(this);

    ui->difficultyComboBox->addItem(tr("Hard"), "hard");
    ui->difficultyComboBox->addItem(tr("Medium"), "medium");
    ui->difficultyComboBox->addItem(tr("Easy"), "easy");

    ui->difficultyComboBox->setCurrentIndex(0);
    mDifficulty = qvariant_cast<QString>(ui->difficultyComboBox->currentData());

    int index = ui->difficultyComboBox->findData(dif);
    if (index >= 0) {
        ui->difficultyComboBox->setCurrentIndex(index);
    }
    ui->tutorialCheckBox->setChecked(tut);
}

MapPropertiesDialog::~MapPropertiesDialog()
{
    delete ui;
}

void MapPropertiesDialog::on_tutorialCheckBox_toggled(bool checked)
{
    mTutorial = checked;
}

void MapPropertiesDialog::on_difficultyComboBox_currentIndexChanged(int index)
{
    mDifficulty = qvariant_cast<QString>(ui->difficultyComboBox->itemData(index));
}
