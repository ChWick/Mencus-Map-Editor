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
class QXmlStreamWriter;

typedef QMap<QString, QString> DATA_PAIRS;
typedef QMultiMap<QString, DATA_PAIRS> CHILD_DATA_PAIRS;
typedef CHILD_DATA_PAIRS::iterator CHILD_DATA_ITERATOR;

namespace Event {
    struct Entry {
        DATA_PAIRS mData;
        CHILD_DATA_PAIRS mChildData;
    };
};

struct EndangeredTile {
    unsigned int mTileType;
    unsigned int mPosX;
    unsigned int mPosY;
};

enum OutputTypes {
    OT_MINIMAL,
    OT_FULL,
};

enum EntityTypes {
    ENTITY_PLAYER       = 1,
    ENTITY_ENEMY        = 2,
    ENTITY_OBJECT       = 4,
};

enum EntityOutput {
    ENT_OUT_ID          = 1,
    ENT_OUT_TYPE        = 2,
    ENT_OUT_POSITION    = 4,

    ENT_OUT_PLAYER      = 4,
    ENT_OUT_FULL        = 2047,

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

    EVENT_LIST mEvents;


    QString getEntityPicturePath() const {return ::getEntityPicturePath(mPrimaryType, mSecondaryType);}
};

enum LinkDirection {
    FIRST_TO_SECOND,
    SECOND_TO_FIRST,
    BOTH,
};

struct LinkEntry {
    QString mId;
    unsigned int mFirstX;
    unsigned int mFirstY;
    unsigned int mSecondX;
    unsigned int mSecondY;
    LinkDirection mLinkDirection;
};

typedef QList<LinkEntry> link_entry_list;

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
    link_entry_list mLinksList;
public:
    Map();
    Map(const QString &sFileName);

    void setFilename(const QString &filename) {mFile.setFileName(filename);}
    void writeToFile(OutputTypes outputType);

    const grid2d<unsigned int> &getTiles() const {return mTiles;}
    grid2d<unsigned int> &getTiles() {return mTiles;}

    const QList<Entity> &getEntities() const {return mEntities;}
    QList<Entity> &getEntities() {return mEntities;}

    const QList<LinkEntry> &getLinksList() const {return mLinksList;}
    QList<LinkEntry> &getLinksList() {return mLinksList;}

    QPointF guiToMap(const QPointF &pos) const;
    QPointF mapToGui(const QPointF &pos) const;

private:
    void readEntity(const QXmlStreamReader &stream, EntityTypes type);
    void writeEntities(QXmlStreamWriter &stream, EntityTypes type, OutputTypes outputType) const;
    void writeEntity(QXmlStreamWriter &stream, EntityTypes type, const Entity &entity, EntityOutput entityOutput, OutputTypes outputType) const;
    void writeEventList(QXmlStreamWriter &stream, const EVENT_LIST &event, OutputTypes outputType) const;
    void writeEvent(QXmlStreamWriter &stream, const Event::Entry &event, OutputTypes outputTypes) const;

};

typedef std::shared_ptr<Map> MapPtr;
#endif // MAP_H
