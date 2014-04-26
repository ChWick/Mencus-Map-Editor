#ifndef MAPAREA_H
#define MAPAREA_H

#include <QScrollArea>
#include <map.h>

class MapArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit MapArea(QWidget *parent = 0);

signals:

public slots:
    void onUpdate(MapPtr);
};

#endif // MAPAREA_H
