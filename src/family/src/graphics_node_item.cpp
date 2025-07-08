#include "graphics_node_item.h"

GraphicsNodeItem::GraphicsNodeItem(const QRectF &slot, QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , highlighted(false)
    , slot(slot)
    , x(slot.left())
    , y(slot.top())
    , w(slot.width())
    , h(slot.height())
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF GraphicsNodeItem::boundingRect() const
{
    return QRectF(x,y,w,h).normalized();
}

void GraphicsNodeItem::setBoundingRect(const QRectF &rect)
{
    if (rect.x() != this->x ||
        rect.y() != this->y ||
        rect.width() != this->w ||
        rect.height() != h)
    {
        this->prepareGeometryChange();
        this->x = rect.x();
        this->y = rect.y();
        this->w = rect.width();
        this->h = rect.height();
    }
}

QPainterPath GraphicsNodeItem::shape() const
{
    QPainterPath path;
    path.addRect(this->boundingRect());
    return path;
}

bool GraphicsNodeItem::contains(const QPointF &point) const
{
    return QAbstractGraphicsShapeItem::contains(point);
}

bool GraphicsNodeItem::isObscuredBy(const QGraphicsItem *item) const
{
    return QAbstractGraphicsShapeItem::isObscuredBy(item);
}

QPainterPath GraphicsNodeItem::opaqueArea() const
{
    return QAbstractGraphicsShapeItem::opaqueArea();
}

QPointF GraphicsNodeItem::getTopAttachPoint() const
{
    return QPointF(this->x + this->w/2.0, this->y);
}

QPointF GraphicsNodeItem::getBottomAttachPoint() const
{
    return QPointF(this->x + this->w/2.0, this->y + this->h);
}

qreal GraphicsNodeItem::getX() const
{
    return this->x;
}

qreal GraphicsNodeItem::getY() const
{
    return this->y;
}

qreal GraphicsNodeItem::getWith() const
{
    return this->w;
}

qreal GraphicsNodeItem::getHeight() const
{
    return this->h;
}

void GraphicsNodeItem::setHighlighted(bool highlighted)
{
    this->highlighted = highlighted;
}
