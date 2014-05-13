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
#include "languageresources.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

class QGraphicsItem;
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
    ENTITY_REGION       = 8,
};

enum EntityOutput {
    ENT_OUT_ID          = 1,
    ENT_OUT_TYPE        = 2,
    ENT_OUT_POSITION    = 4,
    ENT_OUT_SIZE        = 8,
    ENT_OUT_HP          = 16,
    ENT_OUT_DIRECTION   = 32,

    ENT_OUT_PLAYER      = 4 | ENT_OUT_HP | ENT_OUT_DIRECTION,
    ENT_OUT_REGION      = ENT_OUT_ID | ENT_OUT_POSITION | ENT_OUT_SIZE,
    ENT_OUT_OBJECT      = ENT_OUT_ID | ENT_OUT_TYPE | ENT_OUT_POSITION,
    ENT_OUT_ENEMY       = ENT_OUT_OBJECT | ENT_OUT_HP | ENT_OUT_DIRECTION,
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

    QGraphicsItem *mGraphicsItem;

    Entity(QString id, EntityTypes primType, unsigned int secType, const QPointF &pos, const QSizeF &size)
        : mId(id), mPrimaryType(primType), mSecondaryType(secType), mPos(pos), mSize(size), mGraphicsItem(nullptr),
    mEntityOutputFlags(0), mEvents(EVENT_LIST()), mHP(-1), mDirection(1) {

        switch (primType) {
        case ENTITY_PLAYER:
            mEntityOutputFlags = ENT_OUT_PLAYER;
            break;
        case ENTITY_REGION:
            mEntityOutputFlags = ENT_OUT_REGION;
            break;
        case ENTITY_OBJECT:
            mEntityOutputFlags = ENT_OUT_OBJECT;
            break;
        case ENTITY_ENEMY:
            mEntityOutputFlags = ENT_OUT_ENEMY;
            break;
        default:
            mEntityOutputFlags = 0;
            break;
        }
    }
    Entity() {
        mEntityOutputFlags = 0;
        mGraphicsItem = nullptr;
    }

    /*Entity(const Entity &e)
        : mId(e.mId), mPrimaryType(e.mPrimaryType), mSecondaryType(e.mSecondaryType), mPos(e.mPos), mSize(e.mSize),
    mEntityOutputFlags(e.mEntityOutputFlags), {

    }*/

    // additional attributes
    int mEntityOutputFlags;
    EVENT_LIST mEvents;
    float mHP;
    int mDirection;


    QString getEntityPicturePath() const {return ::getEntityPicturePath(mPrimaryType, mSecondaryType);}
};

typedef QSharedPointer<Entity> EntityPtr;
Q_DECLARE_METATYPE( EntityPtr )

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
    bool mActivated;
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
    QList<EntityPtr> mEntities;

    QList<Event::Entry> *mCurrentEventList;
    link_entry_list mLinksList;
    language::LanguageResources mLanguageResources;
public:
    Map();
    Map(const QString &sFileName);

    void setFilename(const QString &filename) {mFile.setFileName(filename);}
    QString getMapName() const {
        QString s(mFile.fileName());
        s = s.left(s.lastIndexOf("."));
        s = s.right(s.length() - s.lastIndexOf("/"));
        return s;
    }
    void writeToFile(OutputTypes outputType);
    void writeToZip(QuaZip &zip);
    QString writeToString(OutputTypes outputType);

    const grid2d<unsigned int> &getTiles() const {return mTiles;}
    grid2d<unsigned int> &getTiles() {return mTiles;}

    const QList<EntityPtr> &getEntities() const {return mEntities;}
    QList<EntityPtr> &getEntities() {return mEntities;}

    const QList<LinkEntry> &getLinksList() const {return mLinksList;}
    QList<LinkEntry> &getLinksList() {return mLinksList;}

    QPointF guiToMap(const QPointF &pos) const;
    QPointF mapToGui(const QPointF &pos) const;

    language::LanguageResources &getLanguageResources() {return mLanguageResources;}

    void resize(int width, int height);

private:
    void readEntity(const QXmlStreamReader &stream, EntityTypes type);
    void writeEntities(QXmlStreamWriter &stream, EntityTypes type, OutputTypes outputType) const;
    void writeEntity(QXmlStreamWriter &stream, EntityTypes type, EntityPtr entity, OutputTypes outputType) const;
    void writeEventList(QXmlStreamWriter &stream, const EVENT_LIST &event, OutputTypes outputType) const;
    void writeEvent(QXmlStreamWriter &stream, const Event::Entry &event, OutputTypes outputTypes) const;

};

typedef QSharedPointer<Map> MapPtr;
#endif // MAP_H
