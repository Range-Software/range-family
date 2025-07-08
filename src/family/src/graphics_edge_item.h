#ifndef GRAPHICS_EDGE_ITEM_H
#define GRAPHICS_EDGE_ITEM_H

#include <QGraphicsPathItem>

#include "graphics_node_item.h"

class GraphicsEdgeItem : public QGraphicsPathItem
{

    protected:

        const GraphicsNodeItem *startItem;
        const GraphicsNodeItem *endItem;
        QPolygonF arrowHead;
        QColor color = Qt::black;

    public:

        GraphicsEdgeItem(const GraphicsNodeItem *startItem, const GraphicsNodeItem *endItem, QGraphicsItem *parent = nullptr);

        QRectF boundingRect() const override;

        QPainterPath shape() const override;

        void updatePosition();

    protected:

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

};

#endif // GRAPHICS_EDGE_ITEM_H
