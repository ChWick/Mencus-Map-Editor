#include "objectsgraphicsview.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QTextStream>

ObjectsGraphicsView::ObjectsGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&mScene);

    addObject(QPointF(0, 0), QSizeF(64, 128), "mencus.png", ENTITY_ENEMY);
}
void ObjectsGraphicsView::mousePressEvent(QMouseEvent *e) {
    if (e->button() != Qt::LeftButton) {return;}

    // get ObjectEntry
    Entity *oe = getObjectEntryAtLocalMousePos(e->pos());
    if (!oe) {return;}
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << oe->mId << static_cast<int>(oe->mPrimaryType) << oe->mSize << oe->mPixmap;
    mimeData->setData("object", data);
    drag->setMimeData(mimeData);
    drag->setPixmap(oe->mGraphicsItem->pixmap());
    drag->setHotSpot(QPoint(0, 0));
    drag->exec();
}

void ObjectsGraphicsView::addObject(const QPointF &pos, const QSizeF &size, const QString &filename, EntityTypes primaryType) {
    QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/objects/%1").arg(filename)));
    pItem->setPos(pos);
    Entity oe = {
        filename,
        primaryType,
        pos,
        size,
        pItem,
        QString("gfx/objects/%1").arg(filename)
    };
    mObjects.push_back(oe);
}
Entity *ObjectsGraphicsView::getObjectEntryAtLocalMousePos(const QPoint &pos) {
    for (Entity &oe : mObjects) {
        if (QRectF(oe.mPos, oe.mSize).contains(pos.x(), pos.y())) {
            return &oe;
        }
    }
    return nullptr;
}
