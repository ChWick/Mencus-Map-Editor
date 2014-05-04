#ifndef EDITEVENTDIALOG_H
#define EDITEVENTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QFormLayout>
#include <QListWidgetItem>
#include "map.h"
#include <QMap>

namespace Ui {
class EditEventDialog;
}



class EditEventDialog : public QDialog
{
    Q_OBJECT
private:
    class ChildDataListWidgetItem : public QListWidgetItem {
    private:
        CHILD_DATA_ITERATOR mData;
    public:
        ChildDataListWidgetItem(QListWidget *pParent, const QString &text, CHILD_DATA_ITERATOR data)
            : QListWidgetItem(text, pParent), mData(data) {

        }
        CHILD_DATA_ITERATOR getData() {return mData;}
    };

    enum PropertyTypes {
        EVENT_TYPE,
        EMITTER_TYPE,
        STRING_TYPE,
        MESSAGE_TYPE,
        BUTTON_TYPE,
        BOOL_TYPE,
    };
    enum LayoutType {
        LAYOUT_EVENT,
        LAYOUT_EMITTER,
    };
    struct DataField {
        PropertyTypes mPropertyType;
        LayoutType mLayoutType;
        QString mID;
        QWidget *mLabel;
        QWidget *mWidget;
        unsigned int mEnabledFlags;
    };

private:
    Event::Entry &mEvent;
    QVector<DataField> mDataFields;
public:
    explicit EditEventDialog(Event::Entry &event, QWidget *parent = 0);
    ~EditEventDialog();

private:
    Ui::EditEventDialog *ui;

    void addProperty(LayoutType lt, const QString &id, const QString &label, PropertyTypes type, unsigned int enabledFlags, bool atBegin = false);

public slots:
    void accept();
    void onAddChildData();
    void onDeleteChildData();
private slots:
    void onEventTypeChanged(int);
    void onEmitterTypeChanged(int);
};

#endif // EDITEVENTDIALOG_H
