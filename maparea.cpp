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
#include <editmapscaledialog.h>
#include <QGraphicsDropShadowEffect>

MapArea::MapArea(QWidget *parent) :
    QGraphicsView(parent)
{
    mMapScale = 0.5;
    mTileSize = 64 * mMapScale;
    mTool = TOOL_PLACE_TILE;
    mGridSize = mTileSize * 0.125f;
    setAcceptDrops(true);
    mLeftPressed = mRightPressed = false;
    this->setScene(&mScene);
    mScene.setBackgroundBrush(QBrush(Qt::black));

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
    mTileSize = mMapScale * 64;
    mGridSize = mTileSize * mGridScale;

    mMap = map;
    this->setScene(&mScene);

    mLinkTiles.clear();
    mLineNumbers.clear();
    mScene.clear();
    mEntitySpecificItems.clear();
    mEndangeredTilesItems.clear();
    if (!map) {return;}

    mScene.setSceneRect(QRect(0, 0, map->getTiles().getSizeX() * mTileSize, map->getTiles().getSizeY() * mTileSize));
    mTiles.resize(map->getTiles().getSizeX(), map->getTiles().getSizeY());
    //pLayout->setGeometry(QRect(0, 0, map->getTiles().getSizeX() * 64, map->getTiles().getSizeY() * 64));
    for (unsigned int x = 0; x < map->getTiles().getSizeX(); x++) {
        for (unsigned int y = 0; y < map->getTiles().getSizeY(); y++) {
            //QWidget *pGV = new QWidget(this);
            //pGV->setStyleSheet(QString("background-image: url(gfx/tiles/Tile%1.png);").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0')));
            //pGV->setFixedSize(64, 64);
            QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0'))));
            pItem->setPos((mMap->mapToGui(QPointF(x, y)) - QPointF(0, 1)) * mTileSize);
            pItem->setScale(mMapScale);
            mTiles(x, y) = pItem;
        }
    }

    for (EntityPtr ent : mMap->getEntities()) {
        if (ent->mPrimaryType == ENTITY_REGION) {
            QGraphicsRectItem *pItem = mScene.addRect(QRectF(QPointF(), ent->mSize));
            pItem->setOpacity(0.6);
            pItem->setBrush(QBrush(Qt::red));
            pItem->setPen(QPen(Qt::transparent));
            pItem->setScale(mTileSize);
            pItem->setPos(mMap->mapToGui(ent->mPos) * mTileSize);
            pItem->setPos(pItem->pos().x(), pItem->pos().y() - (ent->mSize.height() - 1) * mTileSize);
            ent->mGraphicsItem = pItem;
        }
        else {
            QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(ent->getEntityPicturePath()));
            pItem->setPos(mMap->mapToGui(ent->mPos + QPointF(0, ent->mSize.height())) * mTileSize);
            pItem->setScale(mMapScale);
            ent->mGraphicsItem = pItem;
        }
        setPositionFromMapPos(ent->mPos, ent);
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
    if (e->mimeData()->hasFormat("object") || e->mimeData()->hasFormat("object/move")) {

        if (e->keyboardModifiers() & Qt::ControlModifier) {
            e->setDropAction(Qt::CopyAction);
        }
        else {
            e->setDropAction(Qt::MoveAction);
        }
        e->acceptProposedAction();
    }
}
void MapArea::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("object/move")) {
        QByteArray data(event->mimeData()->data("object/move"));
        QDataStream stream(&data, QIODevice::ReadOnly);
        QPointF offset;
        EntityPtr oe;
        stream.readRawData(reinterpret_cast<char*>(&oe), sizeof(EntityPtr*));
        stream >> offset;


        if (event->proposedAction() == Qt::MoveAction) {
            oe->mGraphicsItem->setOpacity(1);
            setPositionFromLocalPos(event->posF() - offset, oe);
        }
        else if (event->proposedAction() == Qt::CopyAction) {
            QGraphicsPixmapItem *pItem = mScene.addPixmap(dynamic_cast<QGraphicsPixmapItem*>(oe->mGraphicsItem)->pixmap());
            pItem->setScale(mMapScale);
            // add object to map
            EntityPtr ent(new Entity(*oe));
            ent->mGraphicsItem = pItem;
            setPositionFromLocalPos(event->posF() - offset, ent);
            emit sigObjectAdded(ent);
        }
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
        pItem->setScale(mMapScale);
        // add object to map
        EntityPtr ent(new Entity(
                       id,
                       static_cast<EntityTypes>(entityType),
                       secondaryType,
                       mMap->guiToMap(event->posF() - hotspot) / mTileSize,
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
        ent->mGraphicsItem = mScene.addRect(QRectF(ent->mPos, ent->mSize), QPen(Qt::transparent), QBrush(Qt::red));
        ent->mGraphicsItem->setOpacity(0.6);
        ent->mGraphicsItem->setScale(mTileSize);
        setPositionFromMapPos(ent->mPos, ent);
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
        QPixmap pixmap = dynamic_cast<QGraphicsPixmapItem*>(oe->mGraphicsItem)->pixmap();
        drag->setPixmap(pixmap.scaled(pixmap.size() * mMapScale));
        drag->setHotSpot(offset.toPoint());

        oe->mGraphicsItem->setOpacity(0.5);
        mScene.update();

        if (Qt::MoveAction != drag->exec(Qt::MoveAction | Qt::CopyAction)) {
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
        if (e->modifiers() & Qt::ShiftModifier) {
            QPoint pos(getTilePosFromRelativeMousePos(e->pos()));
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    placeTileAt(pos + QPoint(x, y));
                }
            }
        }
        else {
            placeTileAt(getTilePosFromRelativeMousePos(e->pos()));
        }
    }
}

void MapArea::wheelEvent(QWheelEvent *e) {
    if (e->phase() == Qt::ScrollUpdate && (e->modifiers() & Qt::ControlModifier) > 0) {
        float deltaAngle = e->angleDelta().y();
        mMapScale *= (deltaAngle > 0) ? 2 : 0.5;
        mMapScale = std::min<float>(mMapScale, 16);
        mMapScale = std::max<float>(mMapScale, 0.125);
        onUpdate(mMap);
        return;
    }
    QGraphicsView::wheelEvent(e);
}

void MapArea::setPositionFromLocalPos(const QPointF &localPos, EntityPtr entity) {
    QPointF pos(localPos + scrollPos());
    if (mGridSize > 0) {
        pos = (pos / mGridSize).toPoint();
        pos *= mGridSize;
    }
    entity->mGraphicsItem->setPos(pos);
    entity->mPos = mMap->guiToMap(pos / mTileSize);
    entity->mPos.ry() -= entity->mSize.height();
}

void MapArea::setPositionFromMapPos(const QPointF &mapPos, EntityPtr entity) {
    if (!entity->mGraphicsItem) {return;}
    entity->mPos = mapPos;
    QPointF gpos = mMap->mapToGui(mapPos);
    gpos.ry() -= (entity->mSize.height());
    gpos *= mTileSize;
    entity->mGraphicsItem->setPos(gpos);
}

void MapArea::placeTileAt(const QPoint &tilePos) {
    if (tilePos.x() < 0 || tilePos.y() < 0 || tilePos.x() >= static_cast<int>(mMap->getTiles().getSizeX()) || tilePos.y() >= static_cast<int>(mMap->getTiles().getSizeY())) {
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
        if (QRectF(oe->mGraphicsItem->pos() - scrollPos(), oe->mSize * mTileSize).contains(pos.x(), pos.y())) {
            offset = pos - oe->mGraphicsItem->pos() + scrollPos();
            return oe;
        }
    }
    return EntityPtr();
}

QPoint MapArea::getTilePosFromRelativeMousePos(const QPoint &pos) {
    return mMap->guiToMap(QPoint((pos.x() + horizontalScrollBar()->value()),
                    pos.y() + verticalScrollBar()->value()), mTileSize) - QPoint(0, 1);
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

    int startX = static_cast<int>(horizontalScrollBar()->value()) / mTileSize;
    int startY = static_cast<int>(verticalScrollBar()->value()) / mTileSize;

    int countX = width() / mTileSize;
    int countY = height() / mTileSize;
    for (int x = 0; x < countX + 1; x++) {
        QGraphicsTextItem *text = mScene.addText(QString("%1").arg(x + startX));
        text->setPos(x * mTileSize + static_cast<int>(horizontalScrollBar()->value() / mTileSize) * mTileSize, verticalScrollBar()->value());
        text->setDefaultTextColor(Qt::white);
        mLineNumbers.push_back(text);
    }
    for (int y = 0; y < countY + 1; y++) {
        QGraphicsTextItem *text = mScene.addText(QString("%1").arg(static_cast<int>(mMap->getTiles().getSizeY()) - y - startY - 1));
        text->setPos(horizontalScrollBar()->value(), y * mTileSize + static_cast<int>(verticalScrollBar()->value() / mTileSize) * mTileSize);
        text->setDefaultTextColor(Qt::white);
        QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(text);
        pEffect->setColor(Qt::black);
        pEffect->setBlurRadius(3);
        pEffect->setOffset(QPointF(0, 0));
        text->setGraphicsEffect(pEffect);
        mLineNumbers.push_back(text);
    }
    mScene.update();
}

void MapArea::onCurrentLinkSelectionChanged(QListWidgetItem *next, QListWidgetItem *) {
    for (auto *p : mLinkTiles) {
        delete p;
    }
    if (!next) {mLinkTiles.resize(0); return;}
    mLinkTiles.resize(2);

    LinksListWidgetItem *pItem = dynamic_cast<LinksListWidgetItem*>(next);
    const LinkEntry &entry = pItem->getEntry();

    mLinkTiles[0] = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(0, 3, 10, QLatin1Char('0'))));
    mLinkTiles[0]->setScale(mMapScale);
    mLinkTiles[0]->setPos(entry.mFirstX * mTileSize, (mMap->getTiles().getSizeY() - entry.mFirstY - 1) * mTileSize);
    mLinkTiles[0]->setOpacity(0.6);

    mLinkTiles[1] = mScene.addPixmap(QPixmap(QString("gfx/tiles/Tile%1.png").arg(0, 3, 10, QLatin1Char('0'))));
    mLinkTiles[1]->setScale(mMapScale);
    mLinkTiles[1]->setPos(entry.mSecondX * mTileSize, (mMap->getTiles().getSizeY() - entry.mSecondY - 1) * mTileSize);
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

    mLinkTiles[0]->setPos(entry.mFirstX * mTileSize, (mMap->getTiles().getSizeY() - entry.mFirstY - 1) * mTileSize);
    mLinkTiles[1]->setPos(entry.mSecondX * mTileSize, (mMap->getTiles().getSizeY() - entry.mSecondY - 1) * mTileSize);
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
            pItem->setScale(mMapScale);
            pItem->setPos(evt.mData["x"].toFloat() * mTileSize, (mMap->getTiles().getSizeY() - evt.mData["y"].toFloat() - 1) * mTileSize);
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
        menu.addAction(tr("Edit map scale"), this, SLOT(onEditMapScale()));
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
        pItem->setPos(tile.mPosX * mTileSize, (mMap->getTiles().getSizeY() - tile.mPosY - 1) * mTileSize);
        pItem->setScale(mMapScale);
        pItem->setOpacity(0.5);
    }
}


void MapArea::onEditMapScale() {
    EditMapScaleDialog dialog(mMapScale, static_cast<double>(mGridSize) / mTileSize, this);
    dialog.exec();
    if (dialog.result() == QDialog::Accepted) {
        mMapScale = dialog.scale();
        mGridScale = dialog.gridScale();
        onUpdate(mMap);
    }
}

void MapArea::onEntityPosOrSizeChanged(EntityPtr ent) {
    if (!ent) {return;}
    setPositionFromMapPos(ent->mPos, ent);
    if (ent->mPrimaryType == ENTITY_REGION) {
        QGraphicsRectItem *item = dynamic_cast<QGraphicsRectItem*>(ent->mGraphicsItem);
        QRectF r(item->rect());
        r.setSize(ent->mSize);
        item->setRect(r);

    }
}
