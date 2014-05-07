#ifndef LINKSLISTWIDGET_H
#define LINKSLISTWIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include "map.h"

class LinksListWidgetItem : public QListWidgetItem {
private:
    LinkEntry mEntry;
public:
    LinksListWidgetItem(const QString &label, QListWidget *parent, const LinkEntry &entry)
        : QListWidgetItem(label, parent),
          mEntry(entry) {
    }
    LinkEntry &getEntry() {return mEntry;}
};

class LinksListWidget : public QListWidget
{
    Q_OBJECT
private:
    MapPtr mMap;
public:
    explicit LinksListWidget(QWidget *parent = 0);

signals:

public slots:
    void onUpdate(MapPtr);
    void onAddLink();
    void onDeleteLink();
};

#endif // LINKSLISTWIDGET_H
