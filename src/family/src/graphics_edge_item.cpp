#include <QPainter>
#include <QPen>
#include <QtMath>

#include "graphics_edge_item.h"

namespace {
    constexpr qreal lineWidth = 2.0;
    constexpr qreal arrowSize = 8.0;
    const QColor lineColor(160, 160, 170);
    const QColor lineColorLight(190, 190, 200);
}

GraphicsEdgeItem::GraphicsEdgeItem(const GraphicsNodeItem *startItem, const GraphicsNodeItem *endItem, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , startItem(startItem)
    , endItem(endItem)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

QRectF GraphicsEdgeItem::boundingRect() const
{
    qreal extra = lineWidth + arrowSize;
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

    QPointF ctrl1(sp.x(), sp.y() + dh * 0.5);
    QPointF ctrl2(ep.x(), ep.y() - dh * 0.5);

    QPainterPath path;
    path.moveTo(sp);
    path.cubicTo(ctrl1, ctrl2, ep);

    this->setPath(path);

    // Calculate arrowhead at the end point
    qreal angle = std::atan2(ep.y() - ctrl2.y(), ep.x() - ctrl2.x());

    QPointF arrowP1 = ep - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                    std::sin(angle + M_PI / 6) * arrowSize);
    QPointF arrowP2 = ep - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                    std::sin(angle - M_PI / 6) * arrowSize);

    arrowHead.clear();
    arrowHead << ep << arrowP1 << arrowP2;
}

void GraphicsEdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    this->updatePosition();

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Draw the curved line with gradient effect
    QLinearGradient gradient(this->path().pointAtPercent(0), this->path().pointAtPercent(1));
    gradient.setColorAt(0.0, lineColorLight);
    gradient.setColorAt(0.5, lineColor);
    gradient.setColorAt(1.0, lineColorLight);

    QPen pen(QBrush(gradient), lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(this->path());

    // Draw arrowhead
    painter->setPen(Qt::NoPen);
    painter->setBrush(lineColor);
    painter->drawPolygon(arrowHead);
}
