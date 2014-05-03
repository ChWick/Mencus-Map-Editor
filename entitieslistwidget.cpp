#include "entitieslistwidget.h"
#include <QListWidgetItem>

EntitiesListWidget::EntitiesListWidget(QWidget *parent) :
    QListWidget(parent)
{
}
void EntitiesListWidget::onUpdate(MapPtr map) {
    mMap = map;
    QListWidgetItem *pItem = new QListWidgetItem(this);
    pItem->setText("Map");
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(map.get())));

    for (Entity &ent : mMap->getEntities()) {
        QListWidgetItem *pItem = new QListWidgetItem(this);
        pItem->setText(ent.mId);
        pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
        pItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(&ent)));
    }
}

void EntitiesListWidget::onItemChanged(QListWidgetItem *pItem) {
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(pItem->data(Qt::UserRole)));
    if (!pEnt) {return;} // initializing
    pEnt->mId = pItem->text();
}

void EntitiesListWidget::onEditSelectedItem() {
}

void EntitiesListWidget::onObjectAdded(Entity* ent) {
    mMap->getEntities().push_back(*ent);
    QListWidgetItem *pItem = new QListWidgetItem(this);
    pItem->setText(ent->mId);
    pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(&mMap->getEntities().back())));
}
void EntitiesListWidget::onDeleteSelection() {
    if (selectedItems().size() == 0) {return;}
    QListWidgetItem *pItem = selectedItems().first();
    Entity pEnt = *static_cast<Entity*>(qvariant_cast<void*>(pItem->data(Qt::UserRole)));
    emit sigEntityDeleted(&pEnt);
}
void EntitiesListWidget::onEntityDeleted(Entity *ent) {
    // start at 1 since map cant be deleted
    for (int i = 1; i < count(); i++) {
        if (item(i)->text() == ent->mId) {
            delete item(i);
            break;
        }
    }
    for (auto it = mMap->getEntities().begin()++; it != mMap->getEntities().end(); it++) {
        if ((*it).mId == ent->mId) {
            mMap->getEntities().erase(it);
            break;
        }
    }
}
