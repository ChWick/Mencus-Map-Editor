#include "tilepreviewgraphicsview.h"
#include <QGraphicsPixmapItem>

TilePreviewGraphicsView::TilePreviewGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&mScene);
}
void TilePreviewGraphicsView::onChangeTile(int iTile) {
    mScene.clear();
    mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(iTile, 3, 10, QLatin1Char('0'))));
}
