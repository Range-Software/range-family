#ifndef GRAPHICS_NODE_ITEM_H
#define GRAPHICS_NODE_ITEM_H

#include <QAbstractGraphicsShapeItem>

class QPainter;

class GraphicsNodeItem : public QAbstractGraphicsShapeItem
{

    protected:

        //! Width of the glow around a highlighted item.
        static constexpr qreal highlightGlowSize = 12.0;

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

        qreal getWidth() const;

        qreal getHeight() const;

        QPointF getCenterPosition() const;

        void setHighlighted(bool highlighted);

    protected:

        //! Paint glow around the box of a highlighted item, so it stands out from the background.
        //! Must be painted before the box itself.
        void paintHighlightGlow(QPainter *painter, const QRectF &boxRect, qreal rounding) const;

};

#endif // GRAPHICS_NODE_ITEM_H
