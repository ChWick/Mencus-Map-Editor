#ifndef OBJECTSGRAPHICSVIEW_H
#define OBJECTSGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QSizeF>
#include <QList>
#include <QGraphicsScene>
#include "map.h"

class ObjectsGraphicsView : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene mScene;
    QList<Entity> mObjects;
public:
    explicit ObjectsGraphicsView(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
private:
    void addObject(const QPointF &pos, const QSizeF &size, EntityTypes primaryType, int secondaryType);
    Entity *getObjectEntryAtLocalMousePos(const QPoint &pos, QPointF &offset);
signals:
    void sigEntityDeleted(Entity*);
public slots:
};

#endif // OBJECTSGRAPHICSVIEW_H
