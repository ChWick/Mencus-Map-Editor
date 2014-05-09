#ifndef EDITTEXTDIALOG_H
#define EDITTEXTDIALOG_H

#include <QDialog>
#include "languageresources.h"
#include <QListWidgetItem>

namespace Ui {
class EditTextDialog;
}

class EditTextDialog : public QDialog
{
    Q_OBJECT
private:
    language::LanguageResources &mResources;
    language::LanguageResources mEditedResources;
public:
    explicit EditTextDialog(language::LanguageResources &res, QWidget *parent = 0);
    ~EditTextDialog();

private:
    Ui::EditTextDialog *ui;

    bool makeIdUnique(QString &text) const;
    void updateStringIds();

public slots:
    void accept();
public slots:
    void onAddText();
    void onDeleteText();
    void onTextIdChanged(QListWidgetItem*);
    void onTextChanged();
    void onCurrentTextIdSelectionChanged(QListWidgetItem*,QListWidgetItem*);
    void onLanguageChanged(QString);
};

#endif // EDITTEXTDIALOG_H
