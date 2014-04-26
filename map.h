#ifndef MAP_H
#define MAP_H

#include <qstring.h>
#include <qfile.h>
#include <qlist.h>
#include "grid2d.h"
#include "memory"
#include <QPointF>
#include <QSizeF>

class QGraphicsPixmapItem;

struct EndangeredTile {
    unsigned int mTileType;
    unsigned int mPosX;
    unsigned int mPosY;
};

enum EntityTypes {
    ENTITY_ENEMY,
};

struct Entity {
    QString mId;
    EntityTypes mPrimaryType;
    QPointF mPos;
    QSizeF mSize;

    QGraphicsPixmapItem *mGraphicsItem;
    QString mPixmap;
};

class Map
{
private:
    QFile mFile;
    QString mDifficulty;
    QString mBackground;
    unsigned int mSizeX;
    unsigned int mSizeY;
    int mInvert;
    grid2d<unsigned int> mTiles;
    QList<EndangeredTile> mEndangeredTiles;
    QList<Entity> mEntities;
public:
    Map(const QString &sFileName);

    const grid2d<unsigned int> &getTiles() const {return mTiles;}
    grid2d<unsigned int> &getTiles() {return mTiles;}

    const QList<Entity> &getEntities() const {return mEntities;}
    QList<Entity> &getEntities() {return mEntities;}
};

typedef std::shared_ptr<Map> MapPtr;
#endif // MAP_H
