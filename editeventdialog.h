#ifndef EDITEVENTDIALOG_H
#define EDITEVENTDIALOG_H

#include <QDialog>
#include "map.h"

namespace Ui {
class EditEventDialog;
}

class EditEventDialog : public QDialog
{
    Q_OBJECT
private:
    Event::Entry &mEvent;
public:
    explicit EditEventDialog(Event::Entry &event, QWidget *parent = 0);
    ~EditEventDialog();

private:
    Ui::EditEventDialog *ui;
};

#endif // EDITEVENTDIALOG_H
