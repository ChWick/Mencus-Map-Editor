#include "objectsgraphicsview.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QTextStream>
#include <QScrollBar>

ObjectsGraphicsView::ObjectsGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&mScene);
    setAcceptDrops(true);

    addObject(QPointF(0, 0), QSizeF(64, 128), ENTITY_PLAYER, 0);
    addObject(QPointF(64, 64), QSizeF(64, 64), ENTITY_ENEMY, 0);
    addObject(QPointF(128, 0), QSizeF(64, 128), ENTITY_ENEMY, 1);
    addObject(QPointF(192, 0), QSizeF(256, 128), ENTITY_ENEMY, 2);
    addObject(QPointF(64, 128), QSizeF(64, 128), ENTITY_ENEMY, 4);

    addObject(QPointF(0, 256), QSizeF(32, 32), ENTITY_OBJECT, 0);
    addObject(QPointF(32, 256), QSizeF(32, 32), ENTITY_OBJECT, 1);
    addObject(QPointF(64, 256), QSizeF(32, 32), ENTITY_OBJECT, 2);
    addObject(QPointF(0, 288), QSizeF(64, 32), ENTITY_OBJECT, 3);
    addObject(QPointF(96, 256), QSizeF(64, 64), ENTITY_OBJECT, 4);
    addObject(QPointF(160, 256), QSizeF(32, 64), ENTITY_OBJECT, 5);
}

void ObjectsGraphicsView::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("object") || e->mimeData()->hasFormat("object/move"))
         e->acceptProposedAction();
}
void ObjectsGraphicsView::dragMoveEvent(QDragMoveEvent *e) {
    if (e->mimeData()->hasFormat("object") || e->mimeData()->hasFormat("object/move"))
        e->acceptProposedAction();
}
void ObjectsGraphicsView::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("object/move")) {
        QByteArray data(event->mimeData()->data("object/move"));
        QDataStream stream(&data, QIODevice::ReadOnly);
        QPointF offset;
        EntityPtr oe;
        stream.readRawData(reinterpret_cast<char*>(&oe), sizeof(EntityPtr));
        stream >> offset;

        emit sigEntityDeleted(oe);
    }

    event->acceptProposedAction();
}
void ObjectsGraphicsView::mousePressEvent(QMouseEvent *e) {
    if (e->button() != Qt::LeftButton) {return;}

    // get ObjectEntry
    QPointF offset;
    Entity *oe = getObjectEntryAtLocalMousePos(e->pos(), offset);
    if (!oe) {return;}
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << oe->mId << static_cast<int>(oe->mPrimaryType) << oe->mSecondaryType << oe->mSize << offset;
    mimeData->setData("object", data);
    drag->setMimeData(mimeData);
    drag->setPixmap(dynamic_cast<QGraphicsPixmapItem*>(oe->mGraphicsItem)->pixmap());
    drag->setHotSpot(offset.toPoint());
    drag->exec();
}

void ObjectsGraphicsView::addObject(const QPointF &pos, const QSizeF &size, EntityTypes primaryType, int secondaryType) {
    QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(getEntityPicturePath(primaryType, secondaryType)));
    pItem->setPos(pos);
    Entity oe(
        "New entity",
        primaryType,
        secondaryType,
        pos,
        size
    );
    oe.mGraphicsItem = pItem;
    mObjects.push_back(oe);
}
Entity *ObjectsGraphicsView::getObjectEntryAtLocalMousePos(const QPoint &pos, QPointF &offset) {
    for (Entity &oe : mObjects) {
        if (QRectF(oe.mPos, oe.mSize).contains(pos.x() + horizontalScrollBar()->value(), pos.y() + verticalScrollBar()->value())) {
            offset = pos - oe.mPos + QPointF(horizontalScrollBar()->value(), verticalScrollBar()->value());
            return &oe;
        }
    }
    return nullptr;
}
