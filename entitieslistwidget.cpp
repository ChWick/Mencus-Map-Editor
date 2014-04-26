#include "entitieslistwidget.h"
#include <QListWidgetItem>

EntitiesListWidget::EntitiesListWidget(QWidget *parent) :
    QListWidget(parent)
{
}
void EntitiesListWidget::onUpdate(MapPtr map) {
    mMap = map;

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
