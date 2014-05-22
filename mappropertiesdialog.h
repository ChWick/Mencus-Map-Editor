#ifndef MAPPROPERTIESDIALOG_H
#define MAPPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class MapPropertiesDialog;
}

class MapPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapPropertiesDialog(const QString &dif, bool tut, QWidget *parent = 0);
    ~MapPropertiesDialog();

    const QString difficulty() const {return mDifficulty;}
    bool tutorial() const {return mTutorial;}

private slots:

    void on_tutorialCheckBox_toggled(bool checked);

    void on_difficultyComboBox_currentIndexChanged(int index);

private:
    Ui::MapPropertiesDialog *ui;
    QString mDifficulty;
    bool mTutorial;
};

#endif // MAPPROPERTIESDIALOG_H
