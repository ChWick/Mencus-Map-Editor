#include "selecttiledialog.h"
#include "ui_selecttiledialog.h"
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QScrollBar>

SelectTileDialog::SelectTileDialog(int initialTile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectTileDialog),
    mSelectedTile(initialTile)
{
    ui->setupUi(this);
    ui->previewTile->setScene(&mPreviewScene);
    ui->gridTiles->setScene(&mGridScene);

    unsigned int iTile = 0;
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 9; x++) {
            QGraphicsPixmapItem *pItem = mGridScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(iTile, 3, 10, QLatin1Char('0'))));
            pItem->setPos(x * 66, y * 66);
            iTile++;
            if (iTile > 128) {
                return;
            }
        }
    }

    setPreviewTile(initialTile);
}

SelectTileDialog::~SelectTileDialog()
{
    delete ui;
}


void SelectTileDialog::mouseReleaseEvent ( QMouseEvent * ) {
}

void SelectTileDialog::mousePressEvent ( QMouseEvent *e ) {
    if (ui->gridTiles->contentsRect().contains(ui->gridTiles->mapFromGlobal(this->mapToGlobal(e->pos()))) == false) {return;}
    int tt = getTileTypeAtMousePos(e->pos());

    setPreviewTile(tt);
}

int SelectTileDialog::getTileTypeAtMousePos(const QPoint &pos) {
    QPoint rp = ui->gridTiles->mapFromGlobal(this->mapToGlobal(pos));
    int x = (rp.x() + ui->gridTiles->horizontalScrollBar()->value())/ 66;
    int y = (rp.y() + ui->gridTiles->verticalScrollBar()->value()) / 66;
    //int x = pos.x() / 64;
    //int y = pos.y() / 64;
    return y * 9 + x;
}


void SelectTileDialog::setPreviewTile(int tile) {
    mSelectedTile = tile;
    mPreviewScene.clear();
    mPreviewScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(tile, 3, 10, QLatin1Char('0'))));
}
