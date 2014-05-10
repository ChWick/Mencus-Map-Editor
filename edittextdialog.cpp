#include "edittextdialog.h"
#include "ui_edittextdialog.h"

EditTextDialog::EditTextDialog(language::LanguageResources &res, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTextDialog),
    mResources(res),
    mEditedResources(res)
{
    ui->setupUi(this);

    for (QString l : mEditedResources.getStringIds()) {
        QListWidgetItem *pItem = new QListWidgetItem(ui->textsListWidget);
        pItem->setText(l);
        pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
        pItem->setData(Qt::UserRole, pItem->text());
    }
    for (QMap<QString, QString>::const_iterator it = mEditedResources.getLanguageIds().cbegin(); it != mEditedResources.getLanguageIds().cend(); it++) {
        ui->languageSelect1ComboBox->addItem(it.value(), it.key());
        ui->languageSelect2ComboBox->addItem(it.value(), it.key());
    }

    QObject::connect(ui->textsListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onTextIdChanged(QListWidgetItem*)));
    QObject::connect(ui->plainTextEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    QObject::connect(ui->plainTextEdit_2, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    QObject::connect(ui->textsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onCurrentTextIdSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
    QObject::connect(ui->languageSelect1ComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onLanguageChanged(QString)));
    QObject::connect(ui->languageSelect2ComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onLanguageChanged(QString)));
}

EditTextDialog::~EditTextDialog()
{
    delete ui;
}

bool EditTextDialog::makeIdUnique(QString &text) const {
    QString out = text;
    int counter = 0;
    while (mEditedResources.getStringIds().contains(out)) {
        out = text + QString("_%1").arg(counter);
        counter++;
    }
    if (out == text) {
        return true;
    }
    text = out;
    return false;
}

void EditTextDialog::accept() {
    mResources = mEditedResources;
    QDialog::accept();
}

void EditTextDialog::onAddText() {
    QListWidgetItem *pItem = new QListWidgetItem(ui->textsListWidget);
    QString str("text");
    //makeIdUnique(str);
    //mEditedResources.getStringIds().push_back(str);
    pItem->setText(str);
    pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
}

void EditTextDialog::onDeleteText() {
    if (ui->textsListWidget->currentItem()) {
        delete ui->textsListWidget->currentItem();
    }
}

void EditTextDialog::updateStringIds() {
    mEditedResources.getStringIds().clear();
    for (int i = 0; i < ui->textsListWidget->count(); i++) {
        mEditedResources.getStringIds().push_back(ui->textsListWidget->item(i)->text());
    }
}

void EditTextDialog::onTextIdChanged(QListWidgetItem*item) {
    QString text(item->text());
    if (!makeIdUnique(text)) {
        item->setText(text);
    }
    else {
        if (item->data(Qt::UserRole).isValid()) {
            for (language::Resources &res : mEditedResources.getLanguageMap()) {
                language::StringData &s = res.getStringDataById(qvariant_cast<QString>(item->data(Qt::UserRole)));

                s.mId = text;
            }
        }
        item->setData(Qt::UserRole, text);
    }
    updateStringIds();
}

void EditTextDialog::onTextChanged() {
    QPlainTextEdit *pText = dynamic_cast<QPlainTextEdit*>(sender());
    if (!ui->textsListWidget->currentItem()) {return;}
    QComboBox *pLanguageSelect = NULL;
    QPlainTextEdit *pOtherText = NULL;
    QComboBox *pOtherLanguageSelect = NULL;
    if (pText == ui->plainTextEdit) {
        pLanguageSelect = ui->languageSelect1ComboBox;
        pOtherLanguageSelect = ui->languageSelect2ComboBox;
        pOtherText = ui->plainTextEdit_2;
    }
    else {
        pLanguageSelect = ui->languageSelect2ComboBox;
        pOtherLanguageSelect = ui->languageSelect1ComboBox;
        pOtherText = ui->plainTextEdit;
    }
    if (pLanguageSelect->currentIndex() == -1) {return;}

    QString languageCode = qvariant_cast<QString>(pLanguageSelect->currentData());
    mEditedResources.getResourceByLanguageId(languageCode).getStringDataById(ui->textsListWidget->currentItem()->text()).mString = pText->toPlainText();

    if (pOtherLanguageSelect->currentText() == pLanguageSelect->currentText()) {
        QObject::disconnect(pOtherText, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
        pOtherText->setPlainText(pText->toPlainText());
        QObject::connect(pOtherText, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    }
}

void EditTextDialog::onCurrentTextIdSelectionChanged(QListWidgetItem*cur,QListWidgetItem*) {
    if (ui->languageSelect1ComboBox->currentIndex() != -1) {
        QString languageCode = qvariant_cast<QString>(ui->languageSelect1ComboBox->currentData());
        ui->plainTextEdit->setPlainText(mEditedResources.getResourceByLanguageId(languageCode).getStringDataById(cur->text()).mString);
    }
    if (ui->languageSelect2ComboBox->currentIndex() != -1) {
        QString languageCode = qvariant_cast<QString>(ui->languageSelect2ComboBox->currentData());
        ui->plainTextEdit_2->setPlainText(mEditedResources.getResourceByLanguageId(languageCode).getStringDataById(cur->text()).mString);
    }
}

void EditTextDialog::onLanguageChanged(QString) {
    if (ui->textsListWidget->currentItem() == nullptr) {return;}

    QComboBox *pCB = dynamic_cast<QComboBox*>(sender());
    QPlainTextEdit *pText = nullptr;
    if (pCB == ui->languageSelect1ComboBox) {
        pText = ui->plainTextEdit;
    }
    else {
        pText = ui->plainTextEdit_2;
    }


    QString languageCode = qvariant_cast<QString>(pCB->currentData());
    pText->setPlainText(mEditedResources.getResourceByLanguageId(languageCode).getStringDataById(ui->textsListWidget->currentItem()->text()).mString);

}
