#include "linkslistwidget.h"

LinksListWidget::LinksListWidget(QWidget *parent) :
    QListWidget(parent)
{
}

void LinksListWidget::onUpdate(MapPtr map) {
    mMap = map;
}

void LinksListWidget::onAddLink() {
    mMap->getLinksList().push_back(LinkEntry());
    new LinksListWidgetItem("new_link", this, mMap->getLinksList().back());
}

void LinksListWidget::onDeleteLink() {
    if (currentItem() == nullptr) {return;}
    LinksListWidgetItem *pItem = dynamic_cast<LinksListWidgetItem*>(currentItem());\
    for (link_entry_list::iterator it = mMap->getLinksList().begin(); it != mMap->getLinksList().end(); it++) {
        if (&(*it) == &pItem->getEntry()) {
            mMap->getLinksList().erase(it);
            break;
        }
    }
    delete pItem;
}
