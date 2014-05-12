#ifndef EVENTSLISTWIDGET_H
#define EVENTSLISTWIDGET_H

#include <QListWidget>
#include "map.h"

class Entity;

class EventsListWidget : public QListWidget
{
    Q_OBJECT
private:
    EntityPtr m_pCurrentEntity;
public:
    explicit EventsListWidget(QWidget *parent = 0);

signals:

public slots:
    void onEditCurrent();
    void onAddEvent();
    void onDeleteEvent();
    void onEntitySelectionChanged(QListWidgetItem *, QListWidgetItem *);
};

#endif // EVENTSLISTWIDGET_H
