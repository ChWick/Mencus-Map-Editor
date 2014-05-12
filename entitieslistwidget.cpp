#include "entitieslistwidget.h"
#include <QListWidgetItem>
#include <QLayout>
#include <QDoubleSpinBox>
#include <QGraphicsPixmapItem>
#include <QComboBox>

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

void EntitiesListWidget::onAddEntityButtonPressed() {
    int index = parent()->parent()->findChild<QComboBox*>("entitiesSelectionComboBox")->currentIndex();
    if (index == 0) {
        Entity ent("new region", ENTITY_REGION, 0, QPointF(0, 0), QSizeF(64, 64));
        emit sigEntityAdded(&ent);
    }
}

void EntitiesListWidget::onItemSelectionChanged(QListWidgetItem *next, QListWidgetItem *) {
    bool bHP = false;
    bool bPos = false;
    bool bSize = false;
    if (next) {
        Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(next->data(Qt::UserRole)));
        bHP = (pEnt->mEntityOutputFlags & ENT_OUT_HP) > 0;
        bPos = (pEnt->mEntityOutputFlags & ENT_OUT_POSITION) > 0;
        bSize = (pEnt->mEntityOutputFlags & ENT_OUT_SIZE) > 0;


        parent()->parent()->findChild<QDoubleSpinBox*>("hitpointsSpinBox")->setValue(pEnt->mHP);
        parent()->parent()->findChild<QDoubleSpinBox*>("xCoordSpinBox")->setValue(pEnt->mPos.x() / 64);
        parent()->parent()->findChild<QDoubleSpinBox*>("yCoordSpinBox")->setValue(mMap->getTiles().getSizeY() - pEnt->mPos.y() / 64 - 1);
        parent()->parent()->findChild<QDoubleSpinBox*>("widthSpinBox")->setValue(pEnt->mSize.width());
        parent()->parent()->findChild<QDoubleSpinBox*>("heightSpinBox")->setValue(pEnt->mSize.height());
    }
    parent()->parent()->findChild<QWidget*>("hitpointsSpinBox")->setEnabled(bHP);
    parent()->parent()->findChild<QWidget*>("hitpointsLabel")->setEnabled(bHP);
    parent()->parent()->findChild<QWidget*>("positionLabel")->setEnabled(bPos);
    parent()->parent()->findChild<QWidget*>("xCoordSpinBox")->setEnabled(bPos);
    parent()->parent()->findChild<QWidget*>("yCoordSpinBox")->setEnabled(bPos);
    parent()->parent()->findChild<QWidget*>("sizeLabel")->setEnabled(bSize);
    parent()->parent()->findChild<QWidget*>("widthSpinBox")->setEnabled(bSize);
    parent()->parent()->findChild<QWidget*>("heightSpinBox")->setEnabled(bSize);
}
void EntitiesListWidget::onHpChanged(double t) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mHP = t;
}

void EntitiesListWidget::onXCoordChanged(double d) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mPos.setX(d * 64);
    if (pEnt->mGraphicsItem) {
        pEnt->mGraphicsItem->setPos(pEnt->mPos);
    }
}

void EntitiesListWidget::onYCoordChanged(double d) {
    if (currentItem() == nullptr) {return;}
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(currentItem()->data(Qt::UserRole)));
    pEnt->mPos.setY((mMap->getTiles().getSizeY() - d - 1) * 64);
    if (pEnt->mGraphicsItem) {
        pEnt->mGraphicsItem->setPos(pEnt->mPos);
    }
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
