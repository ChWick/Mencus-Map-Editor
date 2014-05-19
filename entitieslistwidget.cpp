#include "entitieslistwidget.h"
#include <QListWidgetItem>
#include <QLayout>
#include <QDoubleSpinBox>
#include <QGraphicsPixmapItem>
#include <QComboBox>
#include <QGraphicsRectItem>
#include <QCheckBox>

EntitiesListWidget::EntitiesListWidget(QWidget *parent) :
    QListWidget(parent)
{
    QObject::connect(this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onItemSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
}
void EntitiesListWidget::onUpdate(MapPtr map) {
    clear();

    mMap = map;
    if (!map) {return;}
    QListWidgetItem *pItem = new QListWidgetItem(this);
    pItem->setText("Map");
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pItem->setData(Qt::UserRole, QVariant::fromValue(map.dynamicCast<Entity>()));

    for (EntityPtr ent : mMap->getEntities()) {
        QListWidgetItem *pItem = new QListWidgetItem(this);
        pItem->setText(ent->mId);
        pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
        pItem->setData(Qt::UserRole, QVariant::fromValue(ent));
    }
}

void EntitiesListWidget::onItemChanged(QListWidgetItem *pItem) {
    Entity *pEnt = static_cast<Entity*>(qvariant_cast<void*>(pItem->data(Qt::UserRole)));
    if (!pEnt) {return;} // initializing
    pEnt->mId = pItem->text();
}

void EntitiesListWidget::onEditSelectedItem() {
}

void EntitiesListWidget::onObjectAdded(EntityPtr ent) {
    mMap->getEntities().push_back(ent);
    QListWidgetItem *pItem = new QListWidgetItem(this);
    pItem->setText(ent->mId);
    pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pItem->setData(Qt::UserRole, QVariant::fromValue(mMap->getEntities().back()));
}
void EntitiesListWidget::onDeleteSelection() {
    if (selectedItems().size() == 0) {return;}
    QListWidgetItem *pItem = selectedItems().first();
    EntityPtr pEnt = qvariant_cast<EntityPtr>(pItem->data(Qt::UserRole));
    emit sigEntityDeleted(pEnt);
}
void EntitiesListWidget::onEntityDeleted(EntityPtr ent) {
    // start at 1 since map cant be deleted
    for (int i = 1; i < count(); i++) {
        EntityPtr pEnt = qvariant_cast<EntityPtr>(item(i)->data(Qt::UserRole));
        if (pEnt == ent) {
            delete item(i);
            break;
        }
    }
    for (auto it = mMap->getEntities().begin()++; it != mMap->getEntities().end(); it++) {
        if ((*it) == ent) {
            mMap->getEntities().erase(it);
            break;
        }
    }
}

void EntitiesListWidget::onSelectEntity(EntityPtr ent) {
    if (!ent) {
        setCurrentRow(0);
        return;
    }
    for (int i = 0; i < count(); i++) {
        if (qvariant_cast<EntityPtr>(item(i)->data(Qt::UserRole)) == ent) {
            setCurrentRow(i);
            return;
        }
    }
}

void EntitiesListWidget::onAddEntityButtonPressed() {
    int index = parent()->parent()->findChild<QComboBox*>("entitiesSelectionComboBox")->currentIndex();
    if (index == 0) {
        EntityPtr ent(new Entity("new region", ENTITY_REGION, 0, QPointF(0, 0), QSizeF(1, 1)));
        emit sigEntityAdded(ent);
    }
}

void EntitiesListWidget::onItemSelectionChanged(QListWidgetItem *next, QListWidgetItem *) {
    bool bHP = false;
    bool bPos = false;
    bool bSize = false;
    bool bSwitchFlags = false;
    bool bDeactivableFlag = false;
    bool bTimedFlag = false;
    bool bChangeBlocks = false;
    bool bJumps = false;

    if (next) {
        EntityPtr pEnt = qvariant_cast<EntityPtr>(next->data(Qt::UserRole));
        bHP = (pEnt->mEntityOutputFlags & ENT_OUT_HP) > 0;
        bPos = (pEnt->mEntityOutputFlags & ENT_OUT_POSITION) > 0;
        bSize = (pEnt->mEntityOutputFlags & ENT_OUT_SIZE) > 0;
        bJumps = (pEnt->mEntityOutputFlags & ENT_OUT_JUMPS) > 0;

        parent()->parent()->findChild<QDoubleSpinBox*>("hitpointsSpinBox")->setValue(pEnt->mHP);
        parent()->parent()->findChild<QDoubleSpinBox*>("xCoordSpinBox")->setValue(pEnt->mPos.x());
        parent()->parent()->findChild<QDoubleSpinBox*>("yCoordSpinBox")->setValue(pEnt->mPos.y());
        parent()->parent()->findChild<QDoubleSpinBox*>("widthSpinBox")->setValue(pEnt->mSize.width());
        parent()->parent()->findChild<QDoubleSpinBox*>("heightSpinBox")->setValue(pEnt->mSize.height());
        parent()->parent()->findChild<QCheckBox*>("jumpsCheckBox")->setChecked(pEnt->mJumps);

        // flags for switches
        if (pEnt->mPrimaryType == ENTITY_SWITCH) {
            bSwitchFlags = true;
            bDeactivableFlag = (pEnt->mFlags & SF_DEACTIVATABLE) > 0;
            bTimedFlag = (pEnt->mFlags & SF_TIMED) > 0;
            bChangeBlocks = (pEnt->mFlags & SF_CHANGE_BLOCKS) > 0;


            parent()->parent()->findChild<QCheckBox*>("flagDeactivableCheckBox")->setChecked(bDeactivableFlag);
            parent()->parent()->findChild<QCheckBox*>("flagBlocksCheckBox")->setChecked(bChangeBlocks);
            parent()->parent()->findChild<QCheckBox*>("flagTimedCheckBox")->setChecked(bTimedFlag);
            parent()->parent()->findChild<QDoubleSpinBox*>("timeSpinBox")->setValue(pEnt->mTime);
        }
    }
    parent()->parent()->findChild<QWidget*>("hitpointsSpinBox")->setEnabled(bHP);
    parent()->parent()->findChild<QWidget*>("hitpointsLabel")->setEnabled(bHP);
    parent()->parent()->findChild<QWidget*>("positionLabel")->setEnabled(bPos);
    parent()->parent()->findChild<QWidget*>("xCoordSpinBox")->setEnabled(bPos);
    parent()->parent()->findChild<QWidget*>("yCoordSpinBox")->setEnabled(bPos);
    parent()->parent()->findChild<QWidget*>("sizeLabel")->setEnabled(bSize);
    parent()->parent()->findChild<QWidget*>("widthSpinBox")->setEnabled(bSize);
    parent()->parent()->findChild<QWidget*>("heightSpinBox")->setEnabled(bSize);
    parent()->parent()->findChild<QWidget*>("jumpsCheckBox")->setVisible(bJumps);
    parent()->parent()->findChild<QWidget*>("jumpsLabel")->setVisible(bJumps);


    // switch specific
    parent()->parent()->findChild<QWidget*>("flagDeactivableCheckBox")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("flagDeactivableLabel")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("flagBlocksCheckBox")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("flagBlocksLabel")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("flagTimedCheckBox")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("flagTimedLabel")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("timeSpinBox")->setVisible(bSwitchFlags);
    parent()->parent()->findChild<QWidget*>("timeLabel")->setVisible(bSwitchFlags);

}
void EntitiesListWidget::onHpChanged(double t) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mHP = t;
}

void EntitiesListWidget::onXCoordChanged(double d) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mPos.setX(d);
    emit sigEntityPosOrSizeChanged(pEnt);
}

void EntitiesListWidget::onYCoordChanged(double d) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mPos.setY(d);
    emit sigEntityPosOrSizeChanged(pEnt);
}

void EntitiesListWidget::onHeightChanged(double d) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mSize.setHeight(d);
    emit sigEntityPosOrSizeChanged(pEnt);
}

void EntitiesListWidget::onWidthChanged(double d) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mSize.setWidth(d);
    emit sigEntityPosOrSizeChanged(pEnt);
}

void EntitiesListWidget::onTimedToggled(bool b) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    if (pEnt->mPrimaryType == ENTITY_SWITCH) {
        if (b) pEnt->mFlags |= SF_TIMED;
        else if (pEnt->mFlags & SF_TIMED) pEnt->mFlags -= SF_TIMED;
    }
}

void EntitiesListWidget::onDeactivateableToggled(bool b) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    if (pEnt->mPrimaryType == ENTITY_SWITCH) {
        if (b) pEnt->mFlags |= SF_DEACTIVATABLE;
        else if (pEnt->mFlags & SF_DEACTIVATABLE) pEnt->mFlags -= SF_DEACTIVATABLE;
    }
}

void EntitiesListWidget::onChangeBlocksToggled(bool b) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    if (pEnt->mPrimaryType == ENTITY_SWITCH) {
        if (b) pEnt->mFlags |= SF_CHANGE_BLOCKS;
        else if (pEnt->mFlags & SF_CHANGE_BLOCKS) pEnt->mFlags -= SF_CHANGE_BLOCKS;
    }
}

void EntitiesListWidget::onTimeChanged(double v) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mTime = v;
}

void EntitiesListWidget::onJumpsToggled(bool j) {
    if (currentItem() == nullptr) {return;}
    EntityPtr pEnt = qvariant_cast<EntityPtr>(currentItem()->data(Qt::UserRole));
    pEnt->mJumps = j;
}
