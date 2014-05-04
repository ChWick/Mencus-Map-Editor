#include "editeventdialog.h"
#include "ui_editeventdialog.h"
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

QMap<QString, unsigned int> EVENT_TYPES_MAP;
QMap<QString, unsigned int> EMITTER_TYPES_MAP;
QMap<QString, unsigned int> CHILD_DATA_TYPES_MAP;

EditEventDialog::EditEventDialog(Event::Entry &event, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditEventDialog),
    mEvent(event)
{
    EVENT_TYPES_MAP["message"] = 1;
    EVENT_TYPES_MAP["inject_message"] = 2;

    EMITTER_TYPES_MAP["collision"] = 1024;
    EMITTER_TYPES_MAP["create"] = 2048;
    EMITTER_TYPES_MAP["message_page_chagned"] = 4096;

    CHILD_DATA_TYPES_MAP["page"] = 1;

    ui->setupUi(this);




    addProperty(LAYOUT_EVENT, "id", tr("Id"), STRING_TYPE, (2 << 15) - 1);

    addProperty(LAYOUT_EVENT, "title", tr("Title"), STRING_TYPE, EVENT_TYPES_MAP["message"]);


    addProperty(LAYOUT_EVENT, "msg_type", tr("Message type"), MESSAGE_TYPE, EVENT_TYPES_MAP["inject_message"]);
    addProperty(LAYOUT_EVENT, "msg_button", tr("Message button"), BUTTON_TYPE, EVENT_TYPES_MAP["inject_message"]);
    addProperty(LAYOUT_EVENT, "msg_enabled", tr("Message enabled"), BOOL_TYPE, EVENT_TYPES_MAP["inject_message"]);


    addProperty(LAYOUT_EMITTER, "src_id", tr("Source id"), STRING_TYPE, EMITTER_TYPES_MAP["collision"]);


    addProperty(LAYOUT_CHILD_DATA, "text", tr("Text"), STRING_TYPE, CHILD_DATA_TYPES_MAP["page"]);

    // types are last for enable/disable fields
    addProperty(LAYOUT_EVENT, "type", tr("Type"), EVENT_TYPE, (2 << 15) - 1, true, true);
    addProperty(LAYOUT_EMITTER, "emitter", tr("Type"), EMITTER_TYPE, (2 << 15) - 1, true, true);
}

EditEventDialog::~EditEventDialog()
{
    delete ui;
}

void EditEventDialog::addProperty(LayoutType lt, const QString &id, const QString &label, PropertyTypes type, unsigned int enabledFlags, bool atBegin, bool initVisible) {
    assert(mEvent.mData.count(id) <= 1);
    QString defaultValue;
    if (mEvent.mData.count(id) == 1) {
        defaultValue = mEvent.mData.find(id).value();
    }
    QWidget *pDataWidget = NULL;
    QFormLayout *pLayout = NULL;
    switch (lt) {
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
    if (type == EMITTER_TYPE) {
        QComboBox *pCB = new QComboBox(this);
        QObject::connect(pCB, SIGNAL(currentIndexChanged(int)), this, SLOT(onEmitterTypeChanged(int)));
        QMapIterator<QString, unsigned int> it(EMITTER_TYPES_MAP);
        while(it.hasNext()) {
            it.next();
            pCB->addItem(it.key(), it.value());
        }
        pCB->setCurrentText(defaultValue);

        pDataWidget = pCB;
    }
    else if (type == EVENT_TYPE) {
        QComboBox *pCB = new QComboBox(this);
        QObject::connect(pCB, SIGNAL(currentIndexChanged(int)), this, SLOT(onEventTypeChanged(int)));
        QMapIterator<QString, unsigned int> it(EVENT_TYPES_MAP);
        while(it.hasNext()) {
            it.next();
            pCB->addItem(it.key(), it.value());
        }
        pCB->setCurrentText(defaultValue);

        pDataWidget = pCB;
    }
    else if (type == MESSAGE_TYPE) {
        QComboBox *pCB = new QComboBox(this);
        pCB->addItems(QStringList() << "toggle_touch_input_blink" << "exit_reached");
        pCB->setCurrentText(defaultValue);
        pDataWidget = pCB;
    }
    else if (type == STRING_TYPE) {
        QLineEdit *pLineEdit = new QLineEdit(this);
        pLineEdit->setText(defaultValue);

        pDataWidget = pLineEdit;
    }
    else if (type == BUTTON_TYPE) {
        QComboBox *pCB = new QComboBox(this);
        pCB->addItems(QStringList() << "left" << "right" << "jump" << "attack" << "activate" << "enter_link");
        pCB->setCurrentText(defaultValue);
        pDataWidget = pCB;
    }
    else if (type == BOOL_TYPE) {
        QCheckBox *pCB = new QCheckBox(this);
        pCB->setChecked(defaultValue.toLower() == "true");
        pDataWidget = pCB;
    }
    QLabel *pLabel = new QLabel(label);
    if (atBegin) {
        pLayout->insertRow(0, pLabel, pDataWidget);
    }
    else {
        pLayout->addRow(pLabel, pDataWidget);
    }
    pLabel->setVisible(initVisible);
    pDataWidget->setVisible(initVisible);

    mDataFields.append({
                           type,
                           lt,
                           id,
                           pLabel,
                           pDataWidget,
                           enabledFlags
                       });
}
void EditEventDialog::accept() {
    QDialog::accept();

    ui->childDataListWidget->setCurrentItem(NULL);

    for (const auto &data : mDataFields) {
        if (data.mLayoutType == LAYOUT_EMITTER) {
            continue;
        }
        QString value;
        switch (data.mPropertyType) {
        case EMITTER_TYPE:
        case EVENT_TYPE:
        case MESSAGE_TYPE:
        case BUTTON_TYPE:
            value = dynamic_cast<QComboBox*>(data.mWidget)->currentText();
            break;
        case STRING_TYPE:
            value = dynamic_cast<QLineEdit*>(data.mWidget)->text();
            break;
        case BOOL_TYPE:
            value = (dynamic_cast<QCheckBox*>(data.mWidget)->isChecked()) ? "true" : "false";
            break;
        }

        mEvent.mData.replace(data.mID, value);
    }
}

void EditEventDialog::onEventTypeChanged(int index) {
    QComboBox *pCB = dynamic_cast<QComboBox*>(sender());
    unsigned int flag = pCB->itemData(index).toUInt();

    for (const auto &data : mDataFields) {
        if (data.mLayoutType == LAYOUT_EVENT) {
            data.mWidget->setVisible((flag & data.mEnabledFlags) > 0);
            data.mLabel->setVisible((flag & data.mEnabledFlags) > 0);
        }
    }


    ui->childDataListWidget->clear();
    ui->childDataTypesComboBox->clear();

    if (flag == EVENT_TYPES_MAP["message"]) {
        CHILD_DATA_ITERATOR it = mEvent.mChildData.begin();
        for (;it != mEvent.mChildData.end(); it++) {
            ui->childDataListWidget->addItem(new ChildDataListWidgetItem(ui->childDataListWidget, it.key(), it));
        }

        ui->childDataTypesComboBox->addItem("page");
    }
}

void EditEventDialog::onEmitterTypeChanged(int index) {
    QComboBox *pCB = dynamic_cast<QComboBox*>(sender());
    unsigned int flag = pCB->itemData(index).toUInt();

    for (const auto &data : mDataFields) {
        if (data.mLayoutType == LAYOUT_EMITTER) {
            data.mWidget->setVisible((flag & data.mEnabledFlags) > 0);
            data.mLabel->setVisible((flag & data.mEnabledFlags) > 0);
        }
    }

}

void EditEventDialog::onChildDataSelectionTypeChanged(QListWidgetItem*next, QListWidgetItem*previous) {
    if (previous) {
        ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(previous);
        for (const auto &data : mDataFields) {
            if (data.mLayoutType == LAYOUT_CHILD_DATA) {
                QString value;
                switch (data.mPropertyType) {
                case EMITTER_TYPE:
                case EVENT_TYPE:
                case MESSAGE_TYPE:
                case BUTTON_TYPE:
                    value = dynamic_cast<QComboBox*>(data.mWidget)->currentText();
                    break;
                case STRING_TYPE:
                    value = dynamic_cast<QLineEdit*>(data.mWidget)->text();
                    break;
                case BOOL_TYPE:
                    value = (dynamic_cast<QCheckBox*>(data.mWidget)->isChecked()) ? "true" : "false";
                    break;
                }

                pItem->getData()->replace(data.mID, value);
            }
        }
    }
    if (next) {
        ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(next);
        unsigned int flag = CHILD_DATA_TYPES_MAP[pItem->text()];

        for (const auto &data : mDataFields) {
            if (data.mLayoutType == LAYOUT_CHILD_DATA) {
                data.mWidget->setVisible((flag & data.mEnabledFlags) > 0);
                data.mLabel->setVisible((flag & data.mEnabledFlags) > 0);

                QString value;
                if (pItem->getData()->count(data.mID) > 0) {
                    value = *(pItem->getData()->find(data.mID));
                }

                switch (data.mPropertyType) {
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
            if (data.mLayoutType == LAYOUT_CHILD_DATA) {
                data.mWidget->setVisible(false);
                data.mLabel->setVisible(false);
            }
        }
    }
}

void EditEventDialog::onAddChildData() {
    if (ui->childDataTypesComboBox->currentIndex() < 0) {return;}

    CHILD_DATA_ITERATOR it = mEvent.mChildData.insert(mEvent.mChildData.end(), ui->childDataTypesComboBox->currentText(), DATA_PAIRS());
    ui->childDataListWidget->addItem(new ChildDataListWidgetItem(ui->childDataListWidget, it.key(), it));
}

void EditEventDialog::onDeleteChildData() {
    if (ui->childDataListWidget->currentItem() == nullptr) {return;}

    ChildDataListWidgetItem *pItem = dynamic_cast<ChildDataListWidgetItem*>(ui->childDataListWidget->currentItem());
    mEvent.mChildData.erase(pItem->getData());
    delete pItem;
}
