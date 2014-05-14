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
#include <QDrag>
#include "linkslistwidget.h"
#include <QMenu>
#include <editendangeredtiledialog.h>

MapArea::MapArea(QWidget *parent) :
    QGraphicsView(parent)
{
    mTool = TOOL_PLACE_TILE;
    mGridSize = 8;
    setAcceptDrops(true);
    mLeftPressed = mRightPressed = false;
    this->setScene(&mScene);

    setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onUpdateLineNumbers()));
    QObject::connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onUpdateLineNumbers()));
    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onShowContextMenu(const QPoint &)));
}

MapArea::~MapArea() {
    // disconnect, elsewise these functions will be casted again, that leads to an crash due to an deleted mScene
    QObject::disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onUpdateLineNumbers()));
    QObject::disconnect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onUpdateLineNumbers()));

    for (QGraphicsTextItem *ln : mLineNumbers) {
        delete ln;
    }
    mLineNumbers.clear();
}

void MapArea::onUpdate(MapPtr map) {
    mMap = map;
    this->setScene(&mScene);

    mLinkTiles.clear();
    mLineNumbers.clear();
    mScene.clear();
    mEntitySpecificItems.clear();
    mEndangeredTilesItems.clear();
    if (!map) {return;}

    mScene.setSceneRect(QRect(0, 0, map->getTiles().getSizeX() * 64, map->getTiles().getSizeY() * 64));
    mTiles.resize(map->getTiles().getSizeX(), map->getTiles().getSizeY());
    //pLayout->setGeometry(QRect(0, 0, map->getTiles().getSizeX() * 64, map->getTiles().getSizeY() * 64));
    for (unsigned int x = 0; x < map->getTiles().getSizeX(); x++) {
        for (unsigned int y = 0; y < map->getTiles().getSizeY(); y++) {
            //QWidget *pGV = new QWidget(this);
            //pGV->setStyleSheet(QString("background-image: url(gfx/tiles/Tile%1.png);").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0')));
            //pGV->setFixedSize(64, 64);
            QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0'))));
            pItem->setPos(x * 64, (mMap->getTiles().getSizeY() - y - 1) * 64);
            mTiles(x, y) = pItem;
        }
    }

    for (EntityPtr ent : mMap->getEntities()) {
        QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(ent->getEntityPicturePath()));
        pItem->setPos(ent->mPos);
        ent->mGraphicsItem = pItem;
    }

    onUpdateLineNumbers();
    onUpdateEndageredTiles();

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
        EntityPtr oe;
        stream.readRawData(reinterpret_cast<char*>(&oe), sizeof(EntityPtr*));
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
        // add object to map
        EntityPtr ent(new Entity(
                       id,
                       static_cast<EntityTypes>(entityType),
                       secondaryType,
                       event->posF() - hotspot,
                       size
                   ));
        ent->mGraphicsItem = pItem;
        setPositionFromLocalPos(event->posF() - hotspot, ent);
        emit sigObjectAdded(ent);
    }

    event->acceptProposedAction();
}
void MapArea::onObjectAdded(EntityPtr ent) {
    if (ent->mPrimaryType == ENTITY_REGION) {
        ent->mGraphicsItem = mScene.addRect(QRectF(ent->mPos, ent->mSize), QPen(Qt::red), QBrush(Qt::red));
        ent->mGraphicsItem->setOpacity(0.6);
    }
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
        EntityPtr oe = getObjectEntryAtLocalMousePos(e->pos(), offset);
        emit sigSelectEntity(oe);
        if (!oe) {return;}
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.writeRawData(reinterpret_cast<char*>(&oe), sizeof(EntityPtr*));
        stream << offset;
        mimeData->setData("object/move", data);
        drag->setMimeData(mimeData);
        drag->setPixmap(dynamic_cast<QGraphicsPixmapItem*>(oe->mGraphicsItem)->pixmap());
        drag->setHotSpot(offset.toPoint());

        oe->mGraphicsItem->setOpacity(0.5);
        mScene.update();

        if (Qt::IgnoreAction == drag->exec()) {
            oe->mGraphicsItem->setOpacity(1);
        }
    }
    else if (mTool == TOOL_EDIT) {
        if (mLeftPressed) {
            QPointF offset;
            EntityPtr oe = getObjectEntryAtLocalMousePos(e->pos(), offset);
            emit sigSelectEntity(oe);

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

void MapArea::setPositionFromLocalPos(const QPointF &localPos, EntityPtr entity) {
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

EntityPtr MapArea::getObjectEntryAtLocalMousePos(const QPoint &pos, QPointF &offset) {
    for (EntityPtr oe : mMap->getEntities()) {
        if (QRectF(oe->mPos - scrollPos(), oe->mSize).contains(pos.x(), pos.y())) {
            offset = pos - oe->mPos + scrollPos();
            return oe;
        }
    }
    return EntityPtr();
}

QPoint MapArea::getTilePosFromRelativeMousePos(const QPoint &pos) {
    return QPoint((pos.x() + horizontalScrollBar()->value()) / 64,
                    mMap->getTiles().getSizeY() - (pos.y() + verticalScrollBar()->value()) / 64 - 1);
}

void MapArea::onEntityDeleted(EntityPtr ent) {
    delete ent->mGraphicsItem;
    mScene.update();
}

void MapArea::onUpdateLineNumbers() {
    for (QGraphicsTextItem *ln : mLineNumbers) {
        delete ln;
    }
    mLineNumbers.clear();

    int startX = static_cast<int>(horizontalScrollBar()->value()) / 64;
    int startY = static_cast<int>(verticalScrollBar()->value()) / 64;

    int countX = width() / 64;
    int countY = height() / 64;
    for (unsigned int x = 0; x < countX + 1; x++) {
        QGraphicsTextItem *text = mScene.addText(QString("%1").arg(x + startX));
        text->setPos(x * 64 + static_cast<int>(horizontalScrollBar()->value() / 64) * 64, verticalScrollBar()->value());
        text->setDefaultTextColor(Qt::white);
        mLineNumbers.push_back(text);
    }
    for (unsigned int y = 0; y < countY + 1; y++) {
        QGraphicsTextItem *text = mScene.addText(QString("%1").arg(mMap->getTiles().getSizeY() - y - startY - 1));
        text->setPos(horizontalScrollBar()->value(), y * 64 + static_cast<int>(verticalScrollBar()->value() / 64) * 64);
        text->setDefaultTextColor(Qt::white);
        mLineNumbers.push_back(text);
    }
    mScene.update();
}

void MapArea::onCurrentLinkSelectionChanged(QListWidgetItem *next, QListWidgetItem *) {
    for (auto *p : mLinkTiles) {
        delete p;
    }
    if (!next) {mLinkTiles.resize(0);}
    mLinkTiles.resize(2);

    LinksListWidgetItem *pItem = dynamic_cast<LinksListWidgetItem*>(next);
    const LinkEntry &entry = pItem->getEntry();

    mLinkTiles[0] = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(0, 3, 10, QLatin1Char('0'))));
    mLinkTiles[0]->setPos(entry.mFirstX * 64, (mMap->getTiles().getSizeY() - entry.mFirstY - 1) * 64);
    mLinkTiles[0]->setOpacity(0.6);

    mLinkTiles[1] = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(0, 3, 10, QLatin1Char('0'))));
    mLinkTiles[1]->setPos(entry.mSecondX * 64, (mMap->getTiles().getSizeY() - entry.mSecondY - 1) * 64);
    mLinkTiles[1]->setOpacity(0.6);
}

void MapArea::onCurrentLinkValueChanged(QListWidgetItem *item) {
    if (mLinkTiles.size() == 0) {return;}
    if (!item) {
        for (auto *p : mLinkTiles) {
            delete p;
        }
        mLinkTiles.resize(0);
    }

    LinksListWidgetItem *pItem = dynamic_cast<LinksListWidgetItem*>(item);
    const LinkEntry &entry = pItem->getEntry();

    mLinkTiles[0]->setPos(entry.mFirstX * 64, (mMap->getTiles().getSizeY() - entry.mFirstY - 1) * 64);
    mLinkTiles[1]->setPos(entry.mSecondX * 64, (mMap->getTiles().getSizeY() - entry.mSecondY - 1) * 64);
    mScene.update();
}

void MapArea::onSelectedEntityEventsUpdate(EntityPtr ent) {
    while (mEntitySpecificItems.size() > 0) {
        delete mEntitySpecificItems.first();
        mEntitySpecificItems.pop_front();
    }

    for (Event::Entry &evt : ent->mEvents) {
        if (evt.mData["type"] == "change_tile") {
            QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(evt.mData["id"].toInt(), 3, 10, QLatin1Char('0'))));
            pItem->setPos(evt.mData["x"].toFloat() * 64, (mMap->getTiles().getSizeY() - evt.mData["y"].toFloat() - 1) * 64);
            pItem->setOpacity(0.6);
            mEntitySpecificItems << (pItem);
        }
    }
}


void MapArea::onShowContextMenu(const QPoint &pos) {
    mGridMouseClickPos = getTilePosFromRelativeMousePos(pos);
    if (mTool == TOOL_EDIT) {
        QMenu menu(this);
        menu.addAction(tr("Edit endangered tile"), this, SLOT(onEditEndangeredTile()));
        menu.exec(this->mapToGlobal(pos));
    }
}

void MapArea::onEditEndangeredTile() {
    EndangeredTile *tileFromMap = NULL;
    EndangeredTile tileToEdit;
    tileToEdit.mPosX = mGridMouseClickPos.x();
    tileToEdit.mPosY = mGridMouseClickPos.y();
    for (EndangeredTile &tile : mMap->getEndangeredTilesList()) {
        if (tile.mPosX == mGridMouseClickPos.x() && tile.mPosY == mGridMouseClickPos.y()) {
            tileFromMap = &tile;
            tileToEdit = tile;
            break;
        }
    }

    EditEndangeredTileDialog dialog(tileToEdit, this);
    dialog.exec();
    if (dialog.result() == QDialog::Accepted) {
        if (tileFromMap) {
            *tileFromMap = tileToEdit;
        }
        else {
            mMap->getEndangeredTilesList().push_back(tileToEdit);
        }
        onUpdateEndageredTiles();
    }
}
void MapArea::onUpdateEndageredTiles() {
    while (mEndangeredTilesItems.size() > 0) {
        delete mEndangeredTilesItems.front();
        mEndangeredTilesItems.pop_front();
    }

    for (EndangeredTile &tile : mMap->getEndangeredTilesList()) {
        QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(tile.mTileType, 3, 10, QLatin1Char('0'))));
        pItem->setPos(tile.mPosX * 64, (mMap->getTiles().getSizeY() - tile.mPosY - 1) * 64);
        pItem->setOpacity(0.5);
    }
}
