#include "editeventdialog.h"
#include "ui_editeventdialog.h"
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

QMap<QString, unsigned int> EVENT_TYPES_MAP;
QMap<QString, unsigned int> EMITTER_TYPES_MAP;

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

    ui->setupUi(this);




    addProperty(LAYOUT_EVENT, "id", tr("Id"), STRING_TYPE, 0xFFFFFF);

    addProperty(LAYOUT_EVENT, "title", tr("Title"), STRING_TYPE, EVENT_TYPES_MAP["message"]);


    addProperty(LAYOUT_EVENT, "msg_type", tr("Message type"), MESSAGE_TYPE, EVENT_TYPES_MAP["inject_message"]);
    addProperty(LAYOUT_EVENT, "msg_button", tr("Message button"), BUTTON_TYPE, EVENT_TYPES_MAP["inject_message"]);
    addProperty(LAYOUT_EVENT, "msg_enabled", tr("Message enabled"), BOOL_TYPE, EVENT_TYPES_MAP["inject_message"]);


    addProperty(LAYOUT_EMITTER, "src_id", tr("Source id"), STRING_TYPE, EMITTER_TYPES_MAP["collision"]);



    // types are last for enable/disable fields
    addProperty(LAYOUT_EVENT, "type", tr("Type"), EVENT_TYPE, 0xFFFFFF, true);
    addProperty(LAYOUT_EMITTER, "emitter", tr("Type"), EMITTER_TYPE, 0xFFFFFF, true);
}

EditEventDialog::~EditEventDialog()
{
    delete ui;
}

void EditEventDialog::addProperty(LayoutType lt, const QString &id, const QString &label, PropertyTypes type, unsigned int enabledFlags, bool atBegin) {
    QString defaultValue = mEvent.mData[id];
    QWidget *pDataWidget = NULL;
    QFormLayout *pLayout = NULL;
    switch (lt) {
    case LAYOUT_EMITTER:
        pLayout = dynamic_cast<QFormLayout*>(ui->emitterGroupBox->layout());
        break;
    case LAYOUT_EVENT:
        pLayout = dynamic_cast<QFormLayout*>(ui->eventGroupBox->layout());
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

    for (const auto &data : mDataFields) {
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

        mEvent.mData[data.mID] = value;
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
        QMapIterator<QString, QMap<QString,QString> > it(mEvent.mChildData);
        while(it.hasNext()) {
            it.next();
            ui->childDataListWidget->addItem(it.key());
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

void EditEventDialog::onAddChildData() {
    if (ui->childDataTypesComboBox->currentIndex() < 0) {return;}

    ui->childDataListWidget->addItem(ui->childDataTypesComboBox->currentText());
}

void EditEventDialog::onDeleteChildData() {
    if (ui->childDataListWidget->currentItem() == nullptr) {return;}

    delete ui->childDataListWidget->currentItem();
}
