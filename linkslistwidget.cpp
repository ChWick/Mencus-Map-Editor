#include "linkslistwidget.h"
#include <QSpinBox>
#include <QComboBox>

LinksListWidget::LinksListWidget(QWidget *parent) :
    QListWidget(parent)
{
    QObject::connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onItemChanged(QListWidgetItem*)));
    QObject::connect(this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

void LinksListWidget::onUpdate(MapPtr map) {
    mMap = map;
}

void LinksListWidget::onAddLink() {
    mMap->getLinksList().push_back(LinkEntry());
    LinksListWidgetItem *pItem = new LinksListWidgetItem("new_link", this, mMap->getLinksList().back());
    pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
}

void LinksListWidget::onDeleteLink() {
    if (currentItem() == nullptr) {return;}
    LinksListWidgetItem *pItem = dynamic_cast<LinksListWidgetItem*>(currentItem());
    for (link_entry_list::iterator it = mMap->getLinksList().begin(); it != mMap->getLinksList().end(); it++) {
        if (&(*it) == &pItem->getEntry()) {
            mMap->getLinksList().erase(it);
            break;
        }
    }
    delete pItem;
}

void LinksListWidget::onItemChanged(QListWidgetItem *pItem) {
    LinksListWidgetItem *lwi = dynamic_cast<LinksListWidgetItem*>(pItem);
    lwi->getEntry().mId = pItem->text();
}

void LinksListWidget::onCurrentItemChanged(QListWidgetItem *next, QListWidgetItem*previous) {
    if (previous) {
        LinksListWidgetItem *lwi = dynamic_cast<LinksListWidgetItem*>(previous);
        lwi->getEntry().mFirstX = parentWidget()->findChild<QSpinBox*>("linkFirstPosX")->value();
        lwi->getEntry().mFirstY = parentWidget()->findChild<QSpinBox*>("linkFirstPosY")->value();
        lwi->getEntry().mSecondX = parentWidget()->findChild<QSpinBox*>("linkSecondPosX")->value();
        lwi->getEntry().mSecondY = parentWidget()->findChild<QSpinBox*>("linkSecondPosY")->value();

        lwi->getEntry().mLinkDirection = static_cast<LinkDirection>(parentWidget()->findChild<QComboBox*>("linkDirectionComboBox")->currentIndex());
    }
    if (!next) {return;}
    LinksListWidgetItem *lwi = dynamic_cast<LinksListWidgetItem*>(next);
    parentWidget()->findChild<QSpinBox*>("linkFirstPosX")->setValue(lwi->getEntry().mFirstX);
    parentWidget()->findChild<QSpinBox*>("linkFirstPosY")->setValue(lwi->getEntry().mFirstY);
    parentWidget()->findChild<QSpinBox*>("linkSecondPosX")->setValue(lwi->getEntry().mSecondX);
    parentWidget()->findChild<QSpinBox*>("linkSecondPosY")->setValue(lwi->getEntry().mSecondY);
    parentWidget()->findChild<QComboBox*>("linkDirectionComboBox")->setCurrentIndex(lwi->getEntry().mLinkDirection);
}
