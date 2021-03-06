#ifndef MAPAREA_H
#define MAPAREA_H

#include <map.h>
#include <QList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "grid2d.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include "toolsdefinition.h"
#include <QListWidgetItem>
#include <QGraphicsItemGroup>

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
    Tools mTool;
    unsigned int mGridSize;
    QList<QGraphicsTextItem*> mLineNumbers;
    QVector<QGraphicsPixmapItem*> mLinkTiles;
    QList<QGraphicsItem*> mEntitySpecificItems;
    QList<QGraphicsItem*> mEndangeredTilesItems;
    QPoint mGridMouseClickPos;
    float mMapScale;
    float mTileSize;
    float mGridScale;
public:
    explicit MapArea(QWidget *parent = 0);
    ~MapArea();

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    void mouseReleaseEvent ( QMouseEvent *);
    void mousePressEvent ( QMouseEvent *);
    void mouseMoveEvent ( QMouseEvent *);
    void wheelEvent(QWheelEvent *e);
private:
    QPoint getTilePosFromRelativeMousePos(const QPoint &);
    EntityPtr getObjectEntryAtLocalMousePos(const QPoint &pos, QPointF &offset);
    void setPositionFromLocalPos(const QPointF &localPos, EntityPtr entity);
    void setPositionFromMapPos(const QPointF &mapPos, EntityPtr entity);
    void placeTileAt(const QPoint &tilePos);
    QPoint scrollPos();
signals:
    void sigObjectAdded(EntityPtr);
    void sigEntityDeleted(EntityPtr);
    void sigSelectEntity(EntityPtr);
public slots:
    void onObjectAdded(EntityPtr);
    void onUpdate(MapPtr);
    void onChangeLeftTile(int t) {mLeftTile = t;}
    void onChangeRightTile(int t) {mRightTile = t;}
    void onEntityDeleted(EntityPtr);
    void onToolChanged(Tools tool) {mTool = tool; mLeftPressed = mRightPressed = false;}
    void onSelectMove() {onToolChanged(TOOL_MOVE_OBJECT);}
    void onSelectPlaceTile() {onToolChanged(TOOL_PLACE_TILE);}
    void onSelectEdit() {onToolChanged(TOOL_EDIT);}
    void onUpdateLineNumbers();
    void onCurrentLinkSelectionChanged(QListWidgetItem*,QListWidgetItem*);
    void onCurrentLinkValueChanged(QListWidgetItem*);
    void onSelectedEntityEventsUpdate(EntityPtr);
    void onEditMapScale();
    void onEntityPosOrSizeChanged(EntityPtr);
private slots:
    void onShowContextMenu(const QPoint &);
    void onEditEndangeredTile();
    void onUpdateEndageredTiles();
};

#endif // MAPAREA_H
