#include <QPainter>
#include <QFontMetrics>

#include "graphics_relation_item.h"

GraphicsRelationItem::GraphicsRelationItem(const FRelation &relation, const QRectF &slot, GraphicsNodeItem *parent)
    : GraphicsNodeItem(slot,parent)
    , relation(relation)
{

}

void GraphicsRelationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QFontMetrics fm(painter->font());

    QString name = QObject::tr("Relation") + " (" + QObject::tr("children") + ": " + QLocale().toString(this->relation.getChildren().size()) + ")";
    QString date = QLocale().toString(this->relation.getBegin().getDate().toDate(),QLocale::ShortFormat)
                   + " - "
                   + QLocale().toString(this->relation.getEnd().getDate().toDate(),QLocale::ShortFormat);

    QRect nameBoundingRect = fm.boundingRect(name);
    QRect dateBoundingRect = fm.boundingRect(date);

    qreal rounding = 5.0;
    qreal padding = 5.0;

    qreal w = std::max(nameBoundingRect.width(),dateBoundingRect.width()) + 2*padding;
    qreal h = nameBoundingRect.height() + dateBoundingRect.height() + 4*padding;
    qreal x = this->slot.left() + (this->slot.width() - w) / 2.0;
    qreal y = this->slot.top() + (this->slot.height() - h) / 2.0;

    this->setBoundingRect(QRectF(x,y,w,h));

    painter->setPen(this->isSelected() ? Qt::darkRed : Qt::black);
    painter->setBrush(QBrush(QColor(240,240,240),Qt::SolidPattern));

    if (this->isUnderMouse())
    {
        painter->drawRoundedRect(this->slot,rounding,rounding);
    }

    QColor headerColor(200,200,200);
    if (this->highlighted)
    {
        headerColor = headerColor.darker();
        painter->drawRoundedRect(this->slot,rounding,rounding);
    }

    painter->drawRoundedRect(QRectF(x,y,w,h),rounding,rounding);

    painter->setBrush(QBrush(headerColor,Qt::SolidPattern));
    painter->drawRoundedRect(QRectF(x,y,w,nameBoundingRect.height()+2*padding),rounding,rounding);
    painter->drawText(x+padding,y+fm.ascent()+padding,name);
    painter->drawText(x+padding,y+fm.ascent()+nameBoundingRect.height()+3*padding,date);
}

const QUuid &GraphicsRelationItem::getRelationId() const
{
    return this->relation.getId();
}

