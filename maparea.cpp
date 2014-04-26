#include "maparea.h"
#include "qlayout.h"
#include "qgridlayout.h"

MapArea::MapArea(QWidget *parent) :
    QScrollArea(parent)
{
}
void MapArea::onUpdate(MapPtr map) {
    QGridLayout *pLayout = dynamic_cast<QGridLayout*>(widget()->layout());
    //delete all items
    QLayoutItem* item;
    while ( ( item = pLayout->takeAt( 0 ) ) != NULL )
    {
        delete item->widget();
        delete item;
    }

    for (unsigned int x = 0; x < map->getTiles().getSizeX(); x++) {
        for (unsigned int y = 0; y < map->getTiles().getSizeY(); y++) {
            QWidget *pGV = new QWidget(this);
            pGV->setStyleSheet(QString("background-image: url(gfx/tiles/Tile%1.png);").arg(map->getTiles()(x, y), 3, 10, QLatin1Char('0')));
            pGV->setFixedSize(64, 64);
            pLayout->addWidget(pGV, x, y);
        }
    }
}
