#ifndef OBJECTSGRAPHICSVIEW_H
#define OBJECTSGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QSizeF>
#include <QList>
#include <QGraphicsScene>
#include "map.h"

struct ObjectEntry {
    QGraphicsPixmapItem *mItem;
    QSizeF mSize;
    EntityTypes mPrimaryType;
    QString mPixmap;
};

class ObjectsGraphicsView : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene mScene;
    QList<ObjectEntry> mObjects;
public:
    explicit ObjectsGraphicsView(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *);
private:
    void addObject(const QPointF &pos, const QSizeF &size, const QString &filename, EntityTypes primaryType);
    ObjectEntry *getObjectEntryAtLocalMousePos(const QPoint &pos);
signals:

public slots:
};

#endif // OBJECTSGRAPHICSVIEW_H
