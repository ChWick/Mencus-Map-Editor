#include "maparea.h"
#include "qlayout.h"
#include "qgridlayout.h"
#include <QGraphicsView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QTransform>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDataStream>

MapArea::MapArea(QWidget *parent) :
    QGraphicsView(parent)
{
    setAcceptDrops(true);
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

    for (Entity &ent : mMap->getEntities()) {
        QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(ent.getEntityPicturePath()));
        pItem->setPos(ent.mPos * 64);
    }

    update();
    show();
}
void MapArea::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("object"))
         e->acceptProposedAction();
}
void MapArea::dragMoveEvent(QDragMoveEvent *e) {
    if (e->mimeData()->hasFormat("object"))
        e->acceptProposedAction();
}
void MapArea::dropEvent(QDropEvent *event) {
    QByteArray data(event->mimeData()->data("object"));
    QDataStream stream(&data, QIODevice::ReadOnly);
    int entityType, secondaryType;
    QString id;
    QSizeF size;
    QPoint hotspot;
    stream >> id;
    stream >> entityType;
    stream >> secondaryType;
    stream >> size;
    stream >> hotspot;


    QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(getEntityPicturePath(static_cast<EntityTypes>(entityType), secondaryType)));
    pItem->setPos(event->posF() - hotspot);
    // add object to map
    Entity ent({
                   id,
                   static_cast<EntityTypes>(entityType),
                   secondaryType,
                   event->posF() - hotspot,
                   size,
                   pItem
               });
    emit sigObjectAdded(&ent);

    event->acceptProposedAction();
}
void MapArea::onObjectAdded(Entity *ent) {
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

void MapArea::onEntityDeleted(Entity*ent) {
    delete ent->mGraphicsItem;
    mScene.update();
}
