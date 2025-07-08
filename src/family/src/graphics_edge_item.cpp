#include <QPainter>
#include <QPen>

#include "graphics_edge_item.h"

GraphicsEdgeItem::GraphicsEdgeItem(const GraphicsNodeItem *startItem, const GraphicsNodeItem *endItem, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , startItem(startItem)
    , endItem(endItem)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

QRectF GraphicsEdgeItem::boundingRect() const
{
    qreal extra = (this->pen().width()) / 2.0;
    QPointF sp(this->mapFromItem(this->startItem,this->startItem->getBottomAttachPoint()));
    QPointF ep(this->mapFromItem(this->endItem,this->endItem->getTopAttachPoint()));

    return QRectF(sp,ep).normalized().adjusted(-extra,-extra,extra,extra);
}

QPainterPath GraphicsEdgeItem::shape() const
{
    QPainterPath path = this->QGraphicsPathItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void GraphicsEdgeItem::updatePosition()
{
    QPointF sp(this->mapFromItem(this->startItem,this->startItem->getBottomAttachPoint()));
    QPointF ep(this->mapFromItem(this->endItem,this->endItem->getTopAttachPoint()));

    qreal dh = ep.y() - sp.y();

    QPointF p1(sp.x(),sp.y()+dh/2.0f);
    QPointF p2(ep.x(),ep.y()-dh/2.0f);

    QPainterPath path;

    path.moveTo(sp);
    path.lineTo(p1);
    path.lineTo(p2);
    path.lineTo(ep);

    // this->prepareGeometryChange();

    this->setPath(path);
}

void GraphicsEdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    this->updatePosition();
    painter->setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(this->path());
}
