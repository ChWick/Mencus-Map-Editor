#ifndef MAPAREA_H
#define MAPAREA_H

#include <map.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "grid2d.h"
#include <QGraphicsPixmapItem>

class MapArea : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene mScene;
    MapPtr mMap;
    bool mLeftPressed;
    bool mRightPressed;
    int mLeftTile;
    int mRightTile;
    grid2d<QGraphicsPixmapItem*> mTiles;
public:
    explicit MapArea(QWidget *parent = 0);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    void mouseReleaseEvent ( QMouseEvent *);
    void mousePressEvent ( QMouseEvent *);
    void mouseMoveEvent ( QMouseEvent *);
private:
    QPoint getTilePosFromRelativeMousePos(const QPoint &);
    void placeTileAt(const QPoint &tilePos);
signals:
    void sigObjectAdded(Entity*);
public slots:
    void onObjectAdded(Entity*);
    void onUpdate(MapPtr);
    void onChangeLeftTile(int t) {mLeftTile = t;}
    void onChangeRightTile(int t) {mRightTile = t;}
    void onEntityDeleted(Entity*);
};

#endif // MAPAREA_H
