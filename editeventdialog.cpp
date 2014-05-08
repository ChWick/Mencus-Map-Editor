#include "editeventdialog.h"
#include "ui_editeventdialog.h"
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

using namespace EventData;
EditEventDialog::EditEventDialog(Event::Entry &event, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditEventDialog),
    mEvent(event),
    mEditingEvent(event)
{
    ui->setupUi(this);

    for (QMap<QString, EventAttribute>::const_iterator it = EventDataMask::singleton().eventAttributes().begin();
         it != EventDataMask::singleton().eventAttributes().end(); it++) {
        addProperty(*it);
    }
    for (QMap<QString, ChildEventAttribute>::const_iterator it = EventDataMask::singleton().childEventAttributes().begin();
         it != EventDataMask::singleton().childEventAttributes().end(); it++) {
        addProperty(*it);
    }

    onUpdateVisibility();
}

EditEventDialog::~EditEventDialog()
{
    delete ui;
}

void EditEventDialog::addProperty(const EventAttribute &attribute) {
    assert(mEvent.mData.count(attribute.mID) <= 1);
    QString defaultValue;
    if (mEvent.mData.count(attribute.mID) == 1) {
        defaultValue = mEvent.mData.find(attribute.mID).value();
    }
    QWidget *pDataWidget = NULL;
    QFormLayout *pLayout = NULL;
    switch (attribute.mLayoutType) {
    case LAYOUT_EMITTER:
        pLayout = dynamic_cast<QFormLayout*>(ui->emitterGroupBox->layout());
        break;
    case LAYOUT_EVENT:
        pLayout = dynamic_cast<QFormLayout*>(ui->eventGroupBox->layout());
        break;
    case LAYOUT_CHILD_DATA:
        pLayout = ui->childDataLayout;
        break;
    }
    if (attribute.mPropertyType == SELECT_TYPE) {
        QComboBox *pCB = new QComboBox(this);
        pCB->addItems(attribute.mAllowedValues);
        pCB->setCurrentText(defaultValue);

        QObject::connect(pCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxValueChanged(QString)));
        mEditingEvent.mData.replace(attribute.mID, defaultValue);

        pDataWidget = pCB;
    }
    else if (attribute.mPropertyType == STRING_TYPE) {
        QLineEdit *pLineEdit = new QLineEdit(this);
        pLineEdit->setText(defaultValue);
        QObject::connect(pLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onLineEditValueChanged(QString)));

        pDataWidget = pLineEdit;
    }
    else if (attribute.mPropertyType == BOOL_TYPE) {
        QCheckBox *pCB = new QCheckBox(this);
        pCB->setChecked(defaultValue.toLower() == "true");
        pDataWidget = pCB;

        QObject::connect(pCB, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxValueChanged(bool)));
    }
    QLabel *pLabel = new QLabel(attribute.mLabel);
    bool atBegin = false;
    bool initVisible = true;
    if (atBegin) {
        pLayout->insertRow(0, pLabel, pDataWidget);
    }
    else {
        pLayout->addRow(pLabel, pDataWidget);
    }
    pLabel->setVisible(initVisible);
    pDataWidget->setVisible(initVisible);

    mDataFields.append({
                           &attribute,
                           pLabel,
                           pDataWidget
                       });
}
void EditEventDialog::accept() {
    QDialog::accept();

    mEvent = mEditingEvent;
}

const EditEventDialog::DataField *EditEventDialog::getDataFieldByDataWidget(QWidget *pDataWidget) {
    for (const auto &data : mDataFields) {
        if (data.mWidget == pDataWidget) {
            return &data;
        }
    }
    return nullptr;
}

void EditEventDialog::onComboBoxValueChanged(const QString &value) {
    QComboBox *pCB = dynamic_cast<QComboBox*>(sender());
    const DataField *field(getDataFieldByDataWidget(pCB));
    if (!field) {return;}

    if (field->mEventAttribute->mLayoutType == LAYOUT_EVENT || field->mEventAttribute->mLayoutType == LAYOUT_EMITTER) {
        mEditingEvent.mData.replace(field->mEventAttribute->mID, value);
    }
    else if (field->mEventAttribute->mLayoutType == LAYOUT_CHILD_DATA) {
        if (ui->childDataListWidget->currentItem()) {
            ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(ui->childDataListWidget->currentItem());
            pItem->getData()->replace(field->mEventAttribute->mID, value);
        }
    }

    onUpdateVisibility();
}

void EditEventDialog::onLineEditValueChanged(const QString &value) {
    QLineEdit *pLE = dynamic_cast<QLineEdit*>(sender());
    const DataField *field(getDataFieldByDataWidget(pLE));
    if (!field) {return;}

    if (field->mEventAttribute->mLayoutType == LAYOUT_EVENT || field->mEventAttribute->mLayoutType == LAYOUT_EMITTER) {
        mEditingEvent.mData.replace(field->mEventAttribute->mID, value);
    }
    else if (field->mEventAttribute->mLayoutType == LAYOUT_CHILD_DATA) {
        if (ui->childDataListWidget->currentItem()) {
            ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(ui->childDataListWidget->currentItem());
            pItem->getData()->replace(field->mEventAttribute->mID, value);
        }
    }

    //onUpdateVisibility();
}

void EditEventDialog::onCheckBoxValueChanged(bool value) {
    QCheckBox *pCB = dynamic_cast<QCheckBox*>(sender());
    const DataField *field(getDataFieldByDataWidget(pCB));
    if (!field) {return;}

    if (field->mEventAttribute->mLayoutType == LAYOUT_EVENT || field->mEventAttribute->mLayoutType == LAYOUT_EMITTER) {
        mEditingEvent.mData.replace(field->mEventAttribute->mID, value ? "true" : "false");
    }
    else if (field->mEventAttribute->mLayoutType == LAYOUT_CHILD_DATA) {
        if (ui->childDataListWidget->currentItem()) {
            ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(ui->childDataListWidget->currentItem());
            pItem->getData()->replace(field->mEventAttribute->mID, value ? "true" : "false");
        }
    }

    //onUpdateVisibility();

}

void EditEventDialog::onUpdateVisibility() {
    QListWidgetItem *pSelectedChildItem = ui->childDataListWidget->currentItem();
    for (const auto &data : mDataFields) {
        bool bActive = false;
        if (data.mEventAttribute->mLayoutType == LAYOUT_CHILD_DATA) {
            if (pSelectedChildItem) {
                bActive = dynamic_cast<const ChildEventAttribute*>(data.mEventAttribute)->isActive(mEditingEvent, pSelectedChildItem->text());
            }
        }
        else {
            bActive = data.mEventAttribute->isActive(mEditingEvent);
        }
        data.mWidget->setVisible(bActive);
        data.mLabel->setVisible(bActive);
    }


    ui->childDataListWidget->clear();
    ui->childDataTypesComboBox->clear();

    for (QMap<QString, ChildEventType>::const_iterator it = EventDataMask::singleton().childEventTypes().begin();
         it != EventDataMask::singleton().childEventTypes().end(); it++) {
        if (it->isActive(mEditingEvent)) {
            ui->childDataTypesComboBox->addItem(it.value().mID);
        }
    }
    CHILD_DATA_ITERATOR it = mEvent.mChildData.begin();
    for (;it != mEvent.mChildData.end(); it++) {
        if (ui->childDataTypesComboBox->findText(it.key()) == -1) {continue;}
        ui->childDataListWidget->addItem(new ChildDataListWidgetItem(ui->childDataListWidget, it.key(), it));
    }
}

void EditEventDialog::onChildDataSelectionTypeChanged(QListWidgetItem*next, QListWidgetItem*) {
    if (next) {
        ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(next);

        for (const auto &data : mDataFields) {
            if (data.mEventAttribute->mLayoutType == LAYOUT_CHILD_DATA) {
                const ChildEventAttribute *pAttr = dynamic_cast<const ChildEventAttribute*>(data.mEventAttribute);
                data.mWidget->setVisible(pAttr->isActive(mEditingEvent, pItem->text()));
                data.mLabel->setVisible(pAttr->isActive(mEditingEvent, pItem->text()));

                QString value;
                if (pItem->getData()->count(data.mEventAttribute->mID) > 0) {
                    value = *(pItem->getData()->find(data.mEventAttribute->mID));
                }

                switch (data.mEventAttribute->mPropertyType) {
                case SELECT_TYPE:
                case REPEAT_TYPE:
                case EMITTER_TYPE:
                case EVENT_TYPE:
                case MESSAGE_TYPE:
                case BUTTON_TYPE:
                    dynamic_cast<QComboBox*>(data.mWidget)->setCurrentText(value);
                    break;
                case STRING_TYPE:
                    dynamic_cast<QLineEdit*>(data.mWidget)->setText(value);
                    break;
                case BOOL_TYPE:
                    dynamic_cast<QCheckBox*>(data.mWidget)->setChecked(value.toLower() == "true");
                    break;
                }
            }
        }
    }
    else {
        for (const auto &data : mDataFields) {
            if (data.mEventAttribute->mLayoutType == LAYOUT_CHILD_DATA) {
                data.mWidget->setVisible(false);
                data.mLabel->setVisible(false);
            }
        }
    }
}

void EditEventDialog::onAddChildData() {
    if (ui->childDataTypesComboBox->currentIndex() < 0) {return;}

    CHILD_DATA_ITERATOR it = mEditingEvent.mChildData.insert(mEditingEvent.mChildData.end(), ui->childDataTypesComboBox->currentText(), DATA_PAIRS());
    ui->childDataListWidget->addItem(new ChildDataListWidgetItem(ui->childDataListWidget, it.key(), it));
}

void EditEventDialog::onDeleteChildData() {
    if (ui->childDataListWidget->currentItem() == nullptr) {return;}

    ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(ui->childDataListWidget->currentItem());
    ui->childDataListWidget->setCurrentItem(nullptr);
    mEditingEvent.mChildData.erase(pItem->getData());
    delete pItem;
}
