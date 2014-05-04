#include "eventslistwidget.h"
#include "editeventdialog.h"
#include "map.h"

EventsListWidget::EventsListWidget(QWidget *parent) :
    QListWidget(parent)
{
    m_pCurrentEntity = NULL;
}
void EventsListWidget::onEntitySelectionChanged(QListWidgetItem *next, QListWidgetItem *) {
    clear();
    if (next == NULL) {return;}

    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(next->data(Qt::UserRole)));
    m_pCurrentEntity = pEnt;
    for (auto &evt : pEnt->mEvents) {
        QListWidgetItem *pItem = new QListWidgetItem(this);
        pItem->setText(evt.mData["type"]);
        pItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(&evt)));
    }
}

void EventsListWidget::onEditCurrent() {
    if (!m_pCurrentEntity) {return;}
    if (selectedItems().size() == 0) {return;}

    Event::Entry* event = static_cast<Event::Entry*>(qvariant_cast<void*>(selectedItems().first()->data(Qt::UserRole)));
    EditEventDialog diag(*event, this);
    diag.exec();
}

void EventsListWidget::onAddEvent() {
    if (!m_pCurrentEntity) {return;}
    m_pCurrentEntity->mEvents.push_back(Event::Entry());
    QListWidgetItem *pItem = new QListWidgetItem(this);
    m_pCurrentEntity->mEvents.back().mData["type"] = "new event";
    pItem->setText("new event");
    pItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(&m_pCurrentEntity->mEvents.back())));
}

void EventsListWidget::onDeleteEvent() {
    if (!m_pCurrentEntity) {return;}
    if (selectedItems().size() == 0) {return;}

    QListWidgetItem *pItem = selectedItems().front();
    Event::Entry* event = static_cast<Event::Entry*>(qvariant_cast<void*>(pItem->data(Qt::UserRole)));

    for (auto it = m_pCurrentEntity->mEvents.begin(); it != m_pCurrentEntity->mEvents.end(); it++) {
        if (&(*it) == event) {
            m_pCurrentEntity->mEvents.erase(it);
            break;
        }
    }

    delete pItem;
}
