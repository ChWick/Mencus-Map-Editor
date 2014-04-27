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
#include <QDrag>mPos

MapArea::MapArea(QWidget *parent) :
    QGraphicsView(parent)
{
    mTool = TOOL_MOVE_OBJECT;
    mGridSize = 8;
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
        pItem->setPos(ent.mPos);
        ent.mGraphicsItem = pItem;
    }

    update();
    show();
}
void MapArea::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("object") || e->mimeData()->hasFormat("object/move"))
         e->acceptProposedAction();
}
void MapArea::dragMoveEvent(QDragMoveEvent *e) {
    if (e->mimeData()->hasFormat("object") || e->mimeData()->hasFormat("object/move"))
        e->acceptProposedAction();
}
void MapArea::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("object/move")) {
        QByteArray data(event->mimeData()->data("object/move"));
        QDataStream stream(&data, QIODevice::ReadOnly);
        QPointF offset;
        Entity *oe;
        stream.readRawData(reinterpret_cast<char*>(&oe), sizeof(Entity*));
        stream >> offset;

        oe->mGraphicsItem->setOpacity(1);
        setPositionFromLocalPos(event->posF() - offset, oe);
    }
    else {
        QByteArray data(event->mimeData()->data("object"));
        QDataStream stream(&data, QIODevice::ReadOnly);
        int entityType, secondaryType;
        QString id;
        QSizeF size;
        QPointF hotspot;
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
    }

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

    if (mTool == TOOL_PLACE_TILE) {
        placeTileAt(getTilePosFromRelativeMousePos(e->pos()));
    }
    else if (mTool == TOOL_MOVE_OBJECT) {
        // drag event

        // get ObjectEntry
        QPointF offset;
        Entity *oe = getObjectEntryAtLocalMousePos(e->pos(), offset);
        if (!oe) {return;}
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.writeRawData(reinterpret_cast<char*>(&oe), sizeof(Entity*));
        stream << offset;
        mimeData->setData("object/move", data);
        drag->setMimeData(mimeData);
        drag->setPixmap(oe->mGraphicsItem->pixmap());
        drag->setHotSpot(offset.toPoint());

        oe->mGraphicsItem->setOpacity(0.5);
        mScene.update();

        if (Qt::IgnoreAction == drag->exec()) {
            oe->mGraphicsItem->setOpacity(1);
        }
    }
}
void MapArea::mouseMoveEvent ( QMouseEvent * e) {
    if (!mMap) {return;}
    if (!mLeftPressed && !mRightPressed) {return;}

    if (mTool == TOOL_PLACE_TILE) {
        placeTileAt(getTilePosFromRelativeMousePos(e->pos()));
    }
}

void MapArea::setPositionFromLocalPos(const QPointF &localPos, Entity *entity) {
    QPointF pos(localPos + scrollPos());
    if (mGridSize > 0) {
        pos = (pos / mGridSize).toPoint();
        pos *= mGridSize;
    }
    entity->mGraphicsItem->setPos(pos);
    entity->mPos = pos;
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

QPoint MapArea::scrollPos() {
    return QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
}

Entity *MapArea::getObjectEntryAtLocalMousePos(const QPoint &pos, QPointF &offset) {
    for (Entity &oe : mMap->getEntities()) {
        if (QRectF(oe.mPos - scrollPos(), oe.mSize).contains(pos.x(), pos.y())) {
            offset = pos - oe.mPos + scrollPos();
            return &oe;
        }
    }
    return nullptr;
}

QPoint MapArea::getTilePosFromRelativeMousePos(const QPoint &pos) {
    return QPoint((pos.x() + horizontalScrollBar()->value()) / 64,
                    (pos.y() + verticalScrollBar()->value()) / 64);
}

void MapArea::onEntityDeleted(Entity*ent) {
    delete ent->mGraphicsItem;
    mScene.update();
}
