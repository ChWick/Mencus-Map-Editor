#ifndef MAPAREA_H
#define MAPAREA_H

#include <QScrollArea>
#include <map.h>
#include <QGraphicsScene>
#include <QGraphicsView>

class MapArea : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene mScene;
public:
    explicit MapArea(QWidget *parent = 0);

signals:

public slots:
    void onUpdate(MapPtr);
};

#endif // MAPAREA_H
