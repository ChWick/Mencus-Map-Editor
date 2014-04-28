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
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_OBJECT,
};

enum ObjectTypes {
    OBJECT_BOMB = 0,
    OBJECT_HEALTH_POTION,
    OBJECT_MANA_POTION,
    OBJECT_KEY,
    OBJECT_SCRATCH,
    OBJECT_TORCH,
    OBJECT_FLAG,
};

QString getEntityPicturePath(EntityTypes primaryType, unsigned int scondaryType);

struct Entity {
    QString mId;
    EntityTypes mPrimaryType;
    unsigned int mSecondaryType;
    QPointF mPos;
    QSizeF mSize;

    QGraphicsPixmapItem *mGraphicsItem;

    QString getEntityPicturePath() const {return ::getEntityPicturePath(mPrimaryType, mSecondaryType);}
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

    QPointF guiToMap(const QPointF &pos) const;
    QPointF mapToGui(const QPointF &pos) const;

};

typedef std::shared_ptr<Map> MapPtr;
#endif // MAP_H
