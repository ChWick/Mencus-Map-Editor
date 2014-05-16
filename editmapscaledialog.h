#ifndef EDITMAPSCALEDIALOG_H
#define EDITMAPSCALEDIALOG_H

#include <QDialog>

namespace Ui {
class EditMapScaleDialog;
}

class EditMapScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMapScaleDialog(double initScale, double gridScale, QWidget *parent = 0);
    ~EditMapScaleDialog();

    double scale() const {return mScale;}
    double gridScale() const {return mGridScale;}

private slots:
    void on_tileSizeSpinBox_valueChanged(int arg1);

    void on_scaleSpinBox_valueChanged(double arg1);

    void on_gridFactorSpinBox_editingFinished();

private:
    Ui::EditMapScaleDialog *ui;
    int mTileSize;
    double mScale;
    double mGridScale;
};

#endif // EDITMAPSCALEDIALOG_H
