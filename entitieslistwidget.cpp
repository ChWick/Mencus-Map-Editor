#include "entitieslistwidget.h"
#include <QListWidgetItem>

EntitiesListWidget::EntitiesListWidget(QWidget *parent) :
    QListWidget(parent)
{
    QObject::connect(this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onItemSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
}
void EntitiesListWidget::onUpdate(MapPtr map) {
    clear();

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
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(pItem->data(Qt::UserRole)));
    emit sigEntityDeleted(pEnt);
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
void EntitiesListWidget::onItemSelectionChanged(QListWidgetItem *next, QListWidgetItem *) {
    bool bHP = false;
    if (next) {
        Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(next->data(Qt::UserRole)));
        bHP = (pEnt->mEntityOutputFlags & ENT_OUT_HP);
    }
    parent()->parent()->findChild<QWidget*>("hitpointsLineEdit")->setEnabled(bHP);
    parent()->parent()->findChild<QWidget*>("hitpointsLabel")->setEnabled(bHP);
}
void EntitiesListWidget::onHpChanged(double t) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mHP = t;
}

void EntitiesListWidget::onXCoordChanged(double d) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mPos.setX(d);
}

void EntitiesListWidget::onYCoordChanged(double d) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mPos.setY(d);
}

void EntitiesListWidget::onHeightChanged(double d) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mSize.setHeight(d);
}

void EntitiesListWidget::onWidthChanged(double d) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mSize.setWidth(d);
}
