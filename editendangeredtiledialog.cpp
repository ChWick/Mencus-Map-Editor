#include "editendangeredtiledialog.h"
#include "ui_editendangeredtiledialog.h"

EditEndangeredTileDialog::EditEndangeredTileDialog(EndangeredTile &tile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditEndangeredTileDialog),
    mTile(tile)
{
    ui->setupUi(this);

    ui->posX->setValue(mTile.mPosX);
    ui->posY->setValue(mTile.mPosY);
}

EditEndangeredTileDialog::~EditEndangeredTileDialog()
{
    delete ui;
}

void EditEndangeredTileDialog::accept() {
    //mTile.mPosX = ui->posX->value();
    //mTile.mPosY = ui->posY->value();

    QDialog::accept();
}
