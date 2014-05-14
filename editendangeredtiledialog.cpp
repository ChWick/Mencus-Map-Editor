#include "editendangeredtiledialog.h"
#include "ui_editendangeredtiledialog.h"
#include "selecttiledialog.h"

EditEndangeredTileDialog::EditEndangeredTileDialog(EndangeredTile &tile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditEndangeredTileDialog),
    mTile(tile)
{
    ui->setupUi(this);

    ui->posX->setValue(mTile.mPosX);
    ui->posY->setValue(mTile.mPosY);
    ui->tileButton->setIcon(QIcon(QString("gfx/tiles/Tile%1.png").arg(mTile.mTileType, 3, 10, QLatin1Char('0'))));
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

void EditEndangeredTileDialog::on_tileButton_clicked()
{
    SelectTileDialog dialog(mTile.mTileType, this);
    dialog.exec();
    if (dialog.result() == Accepted) {
        mTile.mTileType = dialog.selectedTile();
        ui->tileButton->setIcon(QIcon(QString("gfx/tiles/Tile%1.png").arg(mTile.mTileType, 3, 10, QLatin1Char('0'))));
        //ui->tileButton->setIconSize(QSize(64, 64));
    }
}
