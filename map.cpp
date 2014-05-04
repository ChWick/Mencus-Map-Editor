#include "map.h"
#include <QXmlStreamReader>
#include <QMessageBox>


QString getEntityPicturePath(EntityTypes primaryType, unsigned int secondaryType) {
    switch (primaryType) {
    case ENTITY_PLAYER:
        return "gfx/objects/mencus.png";
    case ENTITY_ENEMY:
        return QString("gfx/objects/enemy_%1.png").arg(secondaryType);
    case ENTITY_OBJECT:
        switch (secondaryType) {
        case OBJECT_BOMB:
            return "gfx/objects/bomb.png";
        case OBJECT_HEALTH_POTION:
            return "gfx/objects/health_potion.png";
        case OBJECT_MANA_POTION:
            return "gfx/objects/mana_potion.png";
        case OBJECT_KEY:
            return "gfx/objects/key.png";
        case OBJECT_SCRATCH:
            return "gfx/objects/scratch.png";
        case OBJECT_TORCH:
            return "gfx/objects/torch1.png";
        case OBJECT_FLAG:
            return "gfx/objects/flag.png";
        }
        break;
    }
    return "";
}
QSizeF getEntitySize(EntityTypes primaryType, unsigned int secondaryType) {
    switch (primaryType) {
    case ENTITY_PLAYER:
        return QSizeF(64, 128);
    case ENTITY_ENEMY:
        switch (secondaryType) {
        case 0:
            return QSizeF(64, 64);
        case 1:
            return QSizeF(64, 128);
        case 2:
            return QSizeF(128, 256);
        case 4:
            return QSizeF(64, 128);
        default:
            return QSizeF(0, 0);
        }
    case ENTITY_OBJECT:
        switch (secondaryType) {
        case OBJECT_BOMB:
            return QSizeF(32, 32);
        case OBJECT_HEALTH_POTION:
            return QSizeF(32, 32);
        case OBJECT_MANA_POTION:
            return QSizeF(32, 32);
        case OBJECT_KEY:
            return QSizeF(64, 32);
        case OBJECT_SCRATCH:
            return QSizeF(64, 64);
        case OBJECT_TORCH:
            return QSizeF(32, 64);
        case OBJECT_FLAG:
            return QSizeF(64, 128);
        default:
            return QSizeF(0, 0);
        }
    }
}

Map::Map()
    : mFile("New map.xml"),
      mCurrentEventList(&mEvents) {
    mSizeX = 20;
    mSizeY = 20;
    mTiles.resize(mSizeX, mSizeY, 1);
}

Map::Map(const QString &sFileName)
 : mFile(sFileName),
    mCurrentEventList(&mEvents) {

    if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(NULL,
                             __FUNCTION__,
                             "Couldn't open amp",
                             QMessageBox::Ok);
            return;
    }
    QXmlStreamReader xml(&mFile);

    int iCurrentRow = 0;
    while(!xml.atEnd() &&
        !xml.hasError()) {
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xml.readNext();
        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        /* If token is StartElement, we'll see if we can read it.*/
        if(token == QXmlStreamReader::StartElement) {
            /* If it's named persons, we'll go to the next.*/
            if(xml.name() == "map") {
                mDifficulty = xml.attributes().value("difficulty").toString();
                mBackground = xml.attributes().value("background").toString();
                mSizeX = xml.attributes().value("sizex").toUInt();
                mSizeY = xml.attributes().value("sizey").toUInt();
                mTiles.resize(mSizeX, mSizeY);
            }
            /* If it's named person, we'll dig the information from there.*/
            else if(xml.name() == "tiles") {
                mInvert = xml.attributes().value("invert").toInt();
            }
            else if (xml.name() == "row") {
                QString row = xml.attributes().value("tiles").toString();
                QStringList splitted = row.split(" ", QString::SplitBehavior::SkipEmptyParts);
                assert(splitted.size() == (int)mSizeX);
                for (unsigned int x = 0; x < mSizeX; x++) {
                    if (mInvert) {
                        mTiles(x, iCurrentRow) = splitted[x].toUInt();
                    }
                    else {
                        mTiles(x, mSizeY - iCurrentRow - 1) = splitted[x].toUInt();
                    }
                }
                iCurrentRow++;
            }
            else if (xml.name() == "tile") {
                // endangered tile
                mEndangeredTiles.push_back({
                                               xml.attributes().value("targetTile").toUInt(),
                                               xml.attributes().value("x").toUInt(),
                                               xml.attributes().value("y").toUInt()
                                           });
            }
            else if (xml.name() == "enemy") {
                readEntity(xml, ENTITY_ENEMY);
            }
            else if (xml.name() == "object") {
                readEntity(xml, ENTITY_OBJECT);
            }
            else if (xml.name() == "player") {
                readEntity(xml, ENTITY_PLAYER);
            }
            else if (xml.name() == "event") {
                Event::Entry entry;
                mCurrentEventList->push_back(entry);
                for (auto &attribute : xml.attributes()) {
                    mCurrentEventList->back().mData.insert(attribute.name().toString(), attribute.value().toString());
                }
            }
            else if (xml.name() == "page") {
                auto it = mCurrentEventList->back().mChildData.insert(xml.name().toString(), DATA_PAIRS());
                for (auto &attribute : xml.attributes()) {
                    (*it).insert(attribute.name().toString(), attribute.value().toString());
                }
            }
        }
        else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == "enemy"
                || xml.name() == "object"
                || xml.name() == "player") {
                mCurrentEventList = &mEvents;
            }
        }
    }
    /* Error handling. */
    if(xml.hasError()) {
        QMessageBox::critical(NULL,
                              __FILE__,
                              xml.errorString(),
                              QMessageBox::Ok);
    }
    /* Removes any device() or data from the reader
     * and resets its internal state to the initial state. */
    xml.clear();

    mFile.close();
}

QPointF Map::guiToMap(const QPointF &pos) const {
    QPointF out(pos);
    return out;
}
QPointF Map::mapToGui(const QPointF &pos) const {
    QPointF out(pos);
    out.setY(mSizeY - out.y());
    return out;
}

void Map::readEntity(const QXmlStreamReader &xml, EntityTypes entType) {
    int type = xml.attributes().value("type").toInt();
    mEntities.push_back({
                            xml.attributes().value("id").toString(),
                            entType,
                            type,
                            mapToGui(QPointF(xml.attributes().value("x").toFloat(),
                            xml.attributes().value("y").toFloat())) * 64,
                            getEntitySize(entType, type),
                            NULL
                        });
    mEntities.back().mPos.ry() -= mEntities.back().mSize.height();
    mCurrentEventList = &(mEntities.back().mEvents);
}
