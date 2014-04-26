#ifndef ENTITIESLISTWIDGET_H
#define ENTITIESLISTWIDGET_H

#include <QListWidget>
#include "map.h"

class EntitiesListWidget : public QListWidget
{
    Q_OBJECT
private:
    MapPtr mMap;
public:
    explicit EntitiesListWidget(QWidget *parent = 0);

signals:
    void sigEntityDeleted(Entity*);
public slots:
    void onUpdate(MapPtr);
    void onEditSelectedItem();
    void onObjectAdded(Entity*);
    void onDeleteSelection();
    void onEntityDeleted(Entity*);

private slots:
    void onItemChanged(QListWidgetItem *);
};

#endif // ENTITIESLISTWIDGET_H
