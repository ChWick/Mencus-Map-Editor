#ifndef SELECTTILEDIALOG_H
#define SELECTTILEDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class SelectTileDialog;
}

class SelectTileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTileDialog(int initialTile, QWidget *parent = 0);
    ~SelectTileDialog();

    int selectedTile() const {return mSelectedTile;}


protected:
    void mouseReleaseEvent ( QMouseEvent * e );
    void mousePressEvent ( QMouseEvent * e );

private:
    int getTileTypeAtMousePos(const QPoint &pos);
    void setPreviewTile(int tile);

private:
    Ui::SelectTileDialog *ui;
    int mSelectedTile;
    QGraphicsScene mGridScene;
    QGraphicsScene mPreviewScene;
};

#endif // SELECTTILEDIALOG_H
