#ifndef TILESGRAPHICSVIEW_H
#define TILESGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

class TilesGraphicsView : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene mScene;
public:
    explicit TilesGraphicsView(QWidget *parent = 0);

    void init();

protected:
    void mouseReleaseEvent ( QMouseEvent * e );
    void mousePressEvent ( QMouseEvent * e );
private:
    int getTileTypeAtMousePos(const QPoint &pos);
signals:
    void sigChangeLeftTile(int);
    void sigChangeRightTile(int);

public slots:

};

#endif // TILESGRAPHICSVIEW_H
