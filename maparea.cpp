#include "maparea.h"
#include "qlayout.h"
#include "qgridlayout.h"
#include <QGraphicsView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QTransform>

MapArea::MapArea(QWidget *parent) :
    QGraphicsView(parent)
{
    mLeftPressed = mRightPressed = false;
    this->setScene(&mScene);
}
void MapArea::onUpdate(MapPtr map) {
    mMap = map;
    this->setScene(&mScene);

    mScene.clear();
    mScene.setSceneRect(QRect(0, 0, map->getTiles().getSizeX() * 64, map->getTiles().getSizeY() * 64));
    mTiles.resize(map->getTiles().getSizeX(), map->getTiles().getSizeY());
    //pLayout->setGeometry(QRect(0, 0, map->getTiles().getSizeX() * 64, map->getTiles().getSizeY() * 64));
    for (unsigned int x = 0; x < map->getTiles().getSizeX(); x++) {
        for (unsigned int y = 0; y < map->getTiles().getSizeY(); y++) {
            //QWidget *pGV = new QWidget(this);
            //pGV->setStyleSheet(QString("background-image: url(gfx/tiles/Tile%1.png);").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0')));
            //pGV->setFixedSize(64, 64);
            QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0'))));
            pItem->setPos(x * 64, y * 64);
            mTiles(x, y) = pItem;
        }
    }

    update();
    show();
}

void MapArea::mouseReleaseEvent ( QMouseEvent * e ) {
    if (e->button() == Qt::LeftButton) {
        mLeftPressed = false;
    }
    else if (e->button() == Qt::RightButton) {
        mRightPressed = false;
    }
}

void MapArea::mousePressEvent ( QMouseEvent * e ) {
    if (!mMap) {return;}

    if (e->button() == Qt::LeftButton) {
        mLeftPressed = true;
    }
    else if (e->button() == Qt::RightButton) {
        mRightPressed = true;
    }

    placeTileAt(getTilePosFromRelativeMousePos(e->pos()));
}
void MapArea::mouseMoveEvent ( QMouseEvent * e) {
    if (!mMap) {return;}
    if (!mLeftPressed && !mRightPressed) {return;}

    placeTileAt(getTilePosFromRelativeMousePos(e->pos()));
}

void MapArea::placeTileAt(const QPoint &tilePos) {
    if (tilePos.x() < 0 || tilePos.y() < 0 || tilePos.x() >= mMap->getTiles().getSizeX() || tilePos.y() >= mMap->getTiles().getSizeY()) {
        return;
    }
    int tile = -1;
    if (mLeftPressed) {
        tile = mLeftTile;
    }
    else if (mRightPressed) {
        tile = mRightTile;
    }
    if (tile >= 0) {
        mTiles(tilePos.x(), tilePos.y())->setPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(tile, 3, 10, QLatin1Char('0'))));
        mMap->getTiles()(tilePos.x(), tilePos.y()) = tile;
    }
}

QPoint MapArea::getTilePosFromRelativeMousePos(const QPoint &pos) {
    return QPoint((pos.x() + horizontalScrollBar()->value()) / 64,
                    (pos.y() + verticalScrollBar()->value()) / 64);
}
