#include "editeventdialog.h"
#include "ui_editeventdialog.h"

EditEventDialog::EditEventDialog(Event::Entry &event, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditEventDialog),
    mEvent(event)
{
    ui->setupUi(this);
}

EditEventDialog::~EditEventDialog()
{
    delete ui;
}
