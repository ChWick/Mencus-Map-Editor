#include "map.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include "eventdatamask.h"


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
                             "Couldn't open map",
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
            else if (xml.name() == "region") {
                readEntity(xml, ENTITY_REGION);
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

    mLanguageResources.loadFromFileSystem(mFile.fileName().left(mFile.fileName().lastIndexOf("/")));
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
    if (entType == ENTITY_REGION) {
        mEntities.push_back({
                                xml.attributes().value("id").toString(),
                                entType,
                                0,
                                mapToGui(QPointF(xml.attributes().value("x").toFloat(),
                                xml.attributes().value("y").toFloat())) * 64,
                                QSizeF(xml.attributes().value("sizex").toFloat(), xml.attributes().value("sizey").toFloat()) * 64,
                                NULL
                            });
    }
    else {
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
        mEntities.back().mHP = xml.attributes().value("hp").toFloat();
        mEntities.back().mDirection = xml.attributes().value("direction").toInt();
        mEntities.back().mPos.ry() -= mEntities.back().mSize.height();
    }
    mCurrentEventList = &(mEntities.back().mEvents);
}

void Map::writeEntities(QXmlStreamWriter &stream, EntityTypes type, OutputTypes outputType) const {
    int entityOutput = ENT_OUT_FULL;
    switch (type) {
    case ENTITY_PLAYER:
    case ENTITY_REGION:
        // written directly
        switch (type) {
        case ENTITY_PLAYER:
            entityOutput = ENT_OUT_PLAYER;
            break;
        case ENTITY_REGION:
            entityOutput = ENT_OUT_REGION;
            break;
        }

        for (const Entity &entity : mEntities) {
            if (entity.mPrimaryType == type)
                writeEntity(stream, type, entity, entityOutput, outputType);
        }
        return;
    case ENTITY_ENEMY:
        stream.writeStartElement("enemies");
        entityOutput = ENT_OUT_ENEMY;
        break;
    case ENTITY_OBJECT:
        stream.writeStartElement("objects");
        entityOutput = ENT_OUT_OBJECT;
        break;
    }

    for (const Entity &entity : mEntities) {
        if (entity.mPrimaryType == type)
            writeEntity(stream, type, entity, entityOutput, outputType);
    }

    stream.writeEndElement();
}
void Map::writeEntity(QXmlStreamWriter &stream, EntityTypes type, const Entity &entity, int entityOutput, OutputTypes outputType) const {
    if (outputType == OT_FULL) {
        entityOutput = ENT_OUT_FULL;
    }
    float additionalPosOffset = 0;
    switch (type) {
    case ENTITY_PLAYER:
        stream.writeStartElement("player");
        additionalPosOffset = entity.mSize.height() / 64;
        break;
    case ENTITY_ENEMY:
        stream.writeStartElement("enemy");
        stream.writeAttribute("type", QString("%1").arg(entity.mSecondaryType));
        additionalPosOffset = entity.mSize.height() / 64;
        break;
    case ENTITY_OBJECT:
        stream.writeStartElement("object");
        additionalPosOffset = entity.mSize.height() / 64;
        break;
    case ENTITY_REGION:
        stream.writeStartElement("region");
        break;
    }

    if (entityOutput & ENT_OUT_TYPE) {stream.writeAttribute("type", QString("%1").arg(entity.mSecondaryType));}
    if (entityOutput & ENT_OUT_ID) {stream.writeAttribute("id", entity.mId);}
    if (entityOutput & ENT_OUT_POSITION) {
        stream.writeAttribute("x", QString("%1").arg(entity.mPos.x() / 64));
        stream.writeAttribute("y", QString("%1").arg(mTiles.getSizeY() - entity.mPos.y() / 64 - additionalPosOffset));
    }
    if (entityOutput & ENT_OUT_SIZE) {
        stream.writeAttribute("sizex", QString("%1").arg(entity.mSize.width() / 64));
        stream.writeAttribute("sizey", QString("%1").arg(entity.mSize.height() / 64));
    }
    if (entityOutput & ENT_OUT_HP) {
        stream.writeAttribute("hp", QString("%1").arg(entity.mHP));
    }
    if (entityOutput & ENT_OUT_DIRECTION) {
        stream.writeAttribute("direction", QString("%1").arg(entity.mDirection));
    }

    writeEventList(stream, entity.mEvents, outputType);

    stream.writeEndElement();
}

void Map::writeEventList(QXmlStreamWriter &stream, const EVENT_LIST &eventList, OutputTypes outputType) const {
    if (eventList.size() > 0) {
        stream.writeStartElement("events");

        for (const Event::Entry &event : eventList) {
            writeEvent(stream, event, outputType);
        }

        stream.writeEndElement();
    }
}

void Map::writeEvent(QXmlStreamWriter &stream, const Event::Entry &event, OutputTypes outputTypes) const {
    stream.writeStartElement("event");

    //stream.writeAttribute("type", event.mData);
    if (outputTypes == OT_FULL) {
        for (DATA_PAIRS::const_iterator it = event.mData.constBegin(); it != event.mData.constEnd(); it++) {
            stream.writeAttribute(it.key(), it.value());
        }
        if (event.mChildData.size() > 0) {
            for (CHILD_DATA_PAIRS::const_iterator it = event.mChildData.constBegin(); it != event.mChildData.constEnd(); it++) {
                stream.writeStartElement(it.key());
                for (DATA_PAIRS::const_iterator cit = it.value().constBegin(); cit != it.value().constEnd(); cit++) {
                    stream.writeAttribute(cit.key(), cit.value());
                }
                stream.writeEndElement();
            }
        }
    }
    else {
        for (DATA_PAIRS::const_iterator it = event.mData.constBegin(); it != event.mData.constEnd(); it++) {
            if (EventData::EventDataMask::singleton().eventAttributes()[it.key()].isActive(event)) {
                stream.writeAttribute(it.key(), it.value());
            }
        }
        for (CHILD_DATA_PAIRS::const_iterator it = event.mChildData.constBegin(); it != event.mChildData.constEnd(); it++) {
            if (EventData::EventDataMask::singleton().childEventTypes()[it.key()].isActive(event) == false) {continue;}

            stream.writeStartElement(it.key());
            for (DATA_PAIRS::const_iterator cit = it.value().constBegin(); cit != it.value().constEnd(); cit++) {
                if (EventData::EventDataMask::singleton().childEventAttributes()[cit.key()].isActive(event, it.key())) {
                    stream.writeAttribute(cit.key(), cit.value());
                }
            }
            stream.writeEndElement();
        }
    }

    stream.writeEndElement();
}

void Map::writeToFile(OutputTypes outputType) {
    mFile.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);

    mFile.write(writeToString(outputType).toUtf8());

    mFile.close();

    mLanguageResources.writeToFileSystem(mFile.fileName().left(mFile.fileName().lastIndexOf("/")));
}

QString Map::writeToString(OutputTypes outputType) {
    QString s;
    QXmlStreamWriter xmlWriter(&s);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("map");
    xmlWriter.writeAttribute("difficulty", mDifficulty);
    xmlWriter.writeAttribute("background", mBackground);
    xmlWriter.writeAttribute("sizex", QString("%1").arg(mSizeX));
    xmlWriter.writeAttribute("sizey", QString("%1").arg(mSizeY));

    xmlWriter.writeStartElement("tiles");
    xmlWriter.writeAttribute("invert", "0");

    for (int r = mSizeY - 1; r >= 0; --r) {
        xmlWriter.writeStartElement("row");
        QString tiles;
        for (unsigned int c = 0; c < mSizeX; c++) {
            tiles += QString("%1 ").arg(mTiles(c, r));
        }
        xmlWriter.writeAttribute("tiles", tiles);
        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();

    writeEntities(xmlWriter, ENTITY_PLAYER, outputType);

    xmlWriter.writeStartElement("switches");
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("endangeredTiles");
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("links");
    xmlWriter.writeEndElement();

    writeEntities(xmlWriter, ENTITY_ENEMY, outputType);
    writeEntities(xmlWriter, ENTITY_OBJECT, outputType);

    xmlWriter.writeStartElement("camera");
    xmlWriter.writeEndElement();

    writeEntities(xmlWriter, ENTITY_REGION, outputType);

    writeEventList(xmlWriter, mEvents, outputType);

    // End of map
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    return s;
}
