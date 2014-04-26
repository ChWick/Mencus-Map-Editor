#include "tilesgraphicsview.h"
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QScrollBar>

TilesGraphicsView::TilesGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&mScene);

    unsigned int iTile = 0;
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 6; x++) {
            QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(iTile, 3, 10, QLatin1Char('0'))));
            pItem->setPos(x * 66, y * 66);
            iTile++;
            if (iTile > 128) {
                return;
            }
        }
    }
}
void TilesGraphicsView::init() {
    emit sigChangeLeftTile(1);
    emit sigChangeRightTile(2);
}

void TilesGraphicsView::mouseReleaseEvent ( QMouseEvent * ) {
}

void TilesGraphicsView::mousePressEvent ( QMouseEvent *e ) {
    int tt = getTileTypeAtMousePos(e->pos());
    if (e->button() == Qt::LeftButton) {
        emit sigChangeLeftTile(tt);
    }
    else if (e->button() == Qt::RightButton){
        emit sigChangeRightTile(tt);
    }
}

int TilesGraphicsView::getTileTypeAtMousePos(const QPoint &pos) {
    int x = (pos.x() + horizontalScrollBar()->value())/ 66;
    int y = (pos.y() + verticalScrollBar()->value()) / 66;
    //int x = pos.x() / 64;
    //int y = pos.y() / 64;
    return y * 6 + x;
}
