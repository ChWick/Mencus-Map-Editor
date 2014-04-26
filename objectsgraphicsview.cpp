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
    ObjectEntry *oe = getObjectEntryAtLocalMousePos(e->pos());
    if (!oe) {return;}
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << oe->mPrimaryType << oe->mPixmap;
    mimeData->setData("object", data);
    drag->setMimeData(mimeData);
    drag->setPixmap(oe->mItem->pixmap());

    drag->exec();
}

void ObjectsGraphicsView::addObject(const QPointF &pos, const QSizeF &size, const QString &filename, EntityTypes primaryType) {
    QGraphicsPixmapItem *pItem = mScene.addPixmap(QPixmap(QString("gfx/objects/%1").arg(filename)));
    pItem->setPos(pos);
    ObjectEntry oe = {
        pItem,
        size,
        primaryType,
        QString("gfx/objects/%1").arg(filename)
    };
    mObjects.push_back(oe);
}
ObjectEntry *ObjectsGraphicsView::getObjectEntryAtLocalMousePos(const QPoint &pos) {
    for (ObjectEntry &oe : mObjects) {
        if (QRectF(oe.mItem->pos(), oe.mSize).contains(pos.x(), pos.y())) {
            return &oe;
        }
    }
    return nullptr;
}
