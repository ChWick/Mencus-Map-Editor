#include "map.h"
#include <QXmlStreamReader>
#include <QMessageBox>


QString getEntityPicturePath(EntityTypes primaryType, unsigned int mSecondaryType) {
    switch (primaryType) {
    case ENTITY_PLAYER:
        return "gfx/objects/mencus.png";
    case ENTITY_ENEMY:
        return QString("gfx/objects/enemy_%1.png").arg(mSecondaryType);
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
        case 1:
        case 2:
        case 3:
            return QSizeF(64, 128);
        }
    }
}

Map::Map(const QString &sFileName)
 : mFile(sFileName) {
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
                mEntities.push_back({
                                        xml.attributes().value("id").toString(),
                                        ENTITY_ENEMY,
                                        xml.attributes().value("type").toInt(),
                                        mapToGui(QPointF(xml.attributes().value("x").toFloat(),
                                        xml.attributes().value("y").toFloat())),
                                        getEntitySize(ENTITY_ENEMY, 0),
                                        NULL
                                    });
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
