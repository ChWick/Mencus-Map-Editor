#ifndef MAP_H
#define MAP_H

#include <qstring.h>
#include <qfile.h>
#include <qlist.h>
#include "grid2d.h"
#include "memory"

struct EndangeredTile {
    unsigned int mTileType;
    unsigned int mPosX;
    unsigned int mPosY;
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
public:
    Map(const QString &sFileName);

    const grid2d<unsigned int> &getTiles() const {return mTiles;}
};

typedef std::shared_ptr<Map> MapPtr;
#endif // MAP_H
