#ifndef EDITBACKGROUNDTYPEDIALOG_H
#define EDITBACKGROUNDTYPEDIALOG_H

#include <QDialog>

namespace Ui {
class EditBackgroundTypeDialog;
}

class EditBackgroundTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditBackgroundTypeDialog(const QString &initValue, QWidget *parent = 0);
    ~EditBackgroundTypeDialog();

    QString background() const {return mBackgroud;}
private slots:
    void on_backgroundComboBox_currentIndexChanged(int index);

private:
    Ui::EditBackgroundTypeDialog *ui;
    QString mBackgroud;
};

#endif // EDITBACKGROUNDTYPEDIALOG_H
