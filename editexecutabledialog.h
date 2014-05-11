#ifndef EDITEXECUTABLEDIALOG_H
#define EDITEXECUTABLEDIALOG_H

#include <QDialog>

namespace Ui {
class EditExecutableDialog;
}

class EditExecutableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditExecutableDialog(QWidget *parent = 0);
    ~EditExecutableDialog();

private:
    Ui::EditExecutableDialog *ui;
public slots:
    void onSelectWorkingDirectory();
    void onSelectExecutablePath();
private slots:
    void accept();
};

#endif // EDITEXECUTABLEDIALOG_H
