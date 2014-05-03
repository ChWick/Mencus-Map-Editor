#ifndef MAP_H
#define MAP_H

#include <qstring.h>
#include <qfile.h>
#include <qlist.h>
#include "grid2d.h"
#include "memory"
#include <QPointF>
#include <QSizeF>
#include <QMap>

class QGraphicsPixmapItem;
class QXmlStreamReader;

typedef QMap<QString, QString> DATA_PAIRS;

namespace Event {
    struct Entry {
        DATA_PAIRS mData;
        QMap<QString, DATA_PAIRS> mChildData;
    };
};

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
typedef QList<Event::Entry> EVENT_LIST;
struct Entity {
    QString mId;
    EntityTypes mPrimaryType;
    unsigned int mSecondaryType;
    QPointF mPos;
    QSizeF mSize;

    QGraphicsPixmapItem *mGraphicsItem;

    QString getEntityPicturePath() const {return ::getEntityPicturePath(mPrimaryType, mSecondaryType);}

    EVENT_LIST mEvents;
};

class Map : public Entity
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

    QList<Event::Entry> *mCurrentEventList;
public:
    Map(const QString &sFileName);

    const grid2d<unsigned int> &getTiles() const {return mTiles;}
    grid2d<unsigned int> &getTiles() {return mTiles;}

    const QList<Entity> &getEntities() const {return mEntities;}
    QList<Entity> &getEntities() {return mEntities;}

    QPointF guiToMap(const QPointF &pos) const;
    QPointF mapToGui(const QPointF &pos) const;

private:
    void readEntity(const QXmlStreamReader &stream, EntityTypes type);

};

typedef std::shared_ptr<Map> MapPtr;
#endif // MAP_H
