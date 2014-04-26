#ifndef TILEPREVIEWGRAPHICSVIEW_H
#define TILEPREVIEWGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

class TilePreviewGraphicsView : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene mScene;
public:
    explicit TilePreviewGraphicsView(QWidget *parent = 0);

signals:

public slots:
    void onChangeTile(int);
};

#endif // TILEPREVIEWGRAPHICSVIEW_H
