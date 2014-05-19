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
    void sigEntityAdded(EntityPtr);
    void sigEntityDeleted(EntityPtr);
    void sigEntityPosOrSizeChanged(EntityPtr);
public slots:
    void onUpdate(MapPtr);
    void onEditSelectedItem();
    void onObjectAdded(EntityPtr);
    void onDeleteSelection();
    void onEntityDeleted(EntityPtr);
    void onSelectEntity(EntityPtr);
    void onHpChanged(double);
    void onXCoordChanged(double);
    void onYCoordChanged(double);
    void onHeightChanged(double);
    void onWidthChanged(double);
    void onAddEntityButtonPressed();
    void onTimedToggled(bool);
    void onDeactivateableToggled(bool);
    void onChangeBlocksToggled(bool);
    void onTimeChanged(double);
    void onJumpsToggled(bool);

private slots:
    void onItemChanged(QListWidgetItem *);
    void onItemSelectionChanged(QListWidgetItem *, QListWidgetItem *);
};

#endif // ENTITIESLISTWIDGET_H
