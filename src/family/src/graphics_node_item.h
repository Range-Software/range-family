#ifndef GRAPHICS_NODE_ITEM_H
#define GRAPHICS_NODE_ITEM_H

#include <QAbstractGraphicsShapeItem>

class GraphicsNodeItem : public QAbstractGraphicsShapeItem
{

    protected:

        bool highlighted;
        QRectF slot;

    private:

        qreal x;
        qreal y;
        qreal w;
        qreal h;

    public:

        GraphicsNodeItem(const QRectF &slot, QGraphicsItem *parent = nullptr);

        QRectF boundingRect() const override;

        void setBoundingRect(const QRectF &rect);

        QPainterPath shape() const override;

        bool contains(const QPointF &point) const override;

        bool isObscuredBy(const QGraphicsItem *item) const override;

        QPainterPath opaqueArea() const override;

        QPointF getTopAttachPoint() const;

        QPointF getBottomAttachPoint() const;

        qreal getX() const;

        qreal getY() const;

        qreal getWith() const;

        qreal getHeight() const;

        void setHighlighted(bool highlighted);

};

#endif // GRAPHICS_NODE_ITEM_H
