#ifndef EDITEVENTDIALOG_H
#define EDITEVENTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QFormLayout>
#include <QListWidgetItem>
#include "map.h"
#include <QMap>
#include "eventdatamask.h"

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

    struct DataField {
        const EventData::EventAttribute *mEventAttribute;
        QWidget *mLabel;
        QWidget *mWidget;
    };

private:
    Event::Entry &mEvent;
    Event::Entry mEditingEvent;
    QVector<DataField> mDataFields;
public:
    explicit EditEventDialog(Event::Entry &event, QWidget *parent = 0);
    ~EditEventDialog();

private:
    Ui::EditEventDialog *ui;

    void addProperty(const EventData::EventAttribute &attribute);
    const DataField *getDataFieldByDataWidget(QWidget *pDataWidget);

public slots:
    void accept();
    void onAddChildData();
    void onDeleteChildData();
private slots:
    void onUpdateVisibility();
    void onComboBoxValueChanged(const QString &);
    void onLineEditValueChanged(const QString &);
    void onCheckBoxValueChanged(bool);
    void onSpinBoxValueChanged(int);
    void onChildDataSelectionTypeChanged(QListWidgetItem*,QListWidgetItem*);
};

#endif // EDITEVENTDIALOG_H
