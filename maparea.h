#ifndef MAPAREA_H
#define MAPAREA_H

#include <QScrollArea>
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

protected:
    void mouseReleaseEvent ( QMouseEvent *);
    void mousePressEvent ( QMouseEvent *);
    void mouseMoveEvent ( QMouseEvent *);
private:
    QPoint getTilePosFromRelativeMousePos(const QPoint &);
    void placeTileAt(const QPoint &tilePos);
signals:

public slots:
    void onUpdate(MapPtr);
    void onChangeLeftTile(int t) {mLeftTile = t;}
    void onChangeRightTile(int t) {mRightTile = t;}
};

#endif // MAPAREA_H
