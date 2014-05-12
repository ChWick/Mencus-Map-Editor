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
    void sigEntityAdded(Entity*);
    void sigEntityDeleted(Entity*);
public slots:
    void onUpdate(MapPtr);
    void onEditSelectedItem();
    void onObjectAdded(Entity*);
    void onDeleteSelection();
    void onEntityDeleted(Entity*);
    void onHpChanged(double);
    void onXCoordChanged(double);
    void onYCoordChanged(double);
    void onHeightChanged(double);
    void onWidthChanged(double);
    void onAddEntityButtonPressed();

private slots:
    void onItemChanged(QListWidgetItem *);
    void onItemSelectionChanged(QListWidgetItem *, QListWidgetItem *);
};

#endif // ENTITIESLISTWIDGET_H
