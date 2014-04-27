#include "objectsgraphicsview.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QTextStream>

ObjectsGraphicsView::ObjectsGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&mScene);

    addObject(QPointF(0, 0), QSizeF(64, 128), ENTITY_PLAYER, 0);
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
    drag->setPixmap(oe->mGraphicsItem->pixmap());
    drag->setHotSpot(offset.toPoint());
    drag->exec();
}

void ObjectsGraphicsView::addObject(const QPointF &pos, const QSizeF &size, EntityTypes primaryType, int secondaryType) {
    QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(getEntityPicturePath(primaryType, secondaryType)));
    pItem->setPos(pos);
    Entity oe = {
        "New entity",
        primaryType,
        secondaryType,
        pos,
        size,
        pItem
    };
    mObjects.push_back(oe);
}
Entity *ObjectsGraphicsView::getObjectEntryAtLocalMousePos(const QPoint &pos, QPointF &offset) {
    for (Entity &oe : mObjects) {
        if (QRectF(oe.mPos, oe.mSize).contains(pos.x(), pos.y())) {
            offset = pos - oe.mPos;
            return &oe;
        }
    }
    return nullptr;
}
