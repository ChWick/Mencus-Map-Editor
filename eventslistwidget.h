#ifndef EVENTSLISTWIDGET_H
#define EVENTSLISTWIDGET_H

#include <QListWidget>

class Entity;

class EventsListWidget : public QListWidget
{
    Q_OBJECT
private:
    Entity *m_pCurrentEntity;
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
