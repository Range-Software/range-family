#include <QPainter>
#include <QFontMetrics>
#include <QStyleOptionGraphicsItem>

#include "graphics_person_item.h"

GraphicsPersonItem::GraphicsPersonItem(const FPerson &person, const QRectF &slot, GraphicsNodeItem *parent)
    : GraphicsNodeItem(slot,parent)
    , person(person)
{

}

void GraphicsPersonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QFontMetrics fm(painter->font());
    QImage personImage;

    QString name = this->person.getName().toString();
    QString date = QLocale().toString(this->person.getBirth().getDate().toDate(),QLocale::ShortFormat)
                 + " - "
                 + QLocale().toString(this->person.getDeath().getDate().toDate(),QLocale::ShortFormat);

    QRect nameBoundingRect = fm.boundingRect(name);
    QRect dateBoundingRect = fm.boundingRect(date);

    qreal rounding = 5.0;
    qreal padding = 5.0;

    qreal w = std::max(nameBoundingRect.width(),dateBoundingRect.width()) + 2*padding;
    qreal h = nameBoundingRect.height() + dateBoundingRect.height() + 4*padding;
    int iw = 0;
    if (!this->person.getPicture().getData().isEmpty())
    {
        QImage sourceImage = QImage::fromData(this->person.getPicture().getData()).scaled(h,h,Qt::KeepAspectRatio, Qt::SmoothTransformation);

        personImage = QImage(sourceImage.size(), QImage::Format_ARGB32);
        personImage.fill(Qt::transparent);

        QPainter p(&personImage);
        QPainterPath clipPath;
        clipPath.addRoundedRect(sourceImage.rect(), rounding, rounding);
        p.setClipPath(clipPath);
        p.drawImage(0,0,sourceImage);

        iw = personImage.width() + padding;
    }
    w += iw;
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
    if (this->person.getSex().toLower() == FPerson::SexDesc::male.toLower())
    {
        headerColor.setRgb(200,200,255);
    }
    else if (this->person.getSex().toLower() == FPerson::SexDesc::female.toLower())
    {
        headerColor.setRgb(255,200,200);
    }
    else if (this->person.getSex().toLower() == FPerson::SexDesc::other.toLower())
    {
        headerColor.setRgb(255,255,200);
    }
    if (this->highlighted)
    {
        headerColor = headerColor.darker();
        painter->drawRoundedRect(this->slot,rounding,rounding);
    }

    painter->drawRoundedRect(QRectF(x,y,w,h),rounding,rounding);

    painter->setBrush(QBrush(headerColor,Qt::SolidPattern));
    painter->drawRoundedRect(QRectF(x,y,w,nameBoundingRect.height()+2*padding),rounding,rounding);

    if (!personImage.isNull())
    {
        painter->drawImage(x,y,personImage);
        painter->setBrush(QBrush(headerColor,Qt::NoBrush));
        painter->drawRoundedRect(QRectF(x,y,personImage.width(),personImage.height()),rounding,rounding);
    }

    painter->drawText(x+padding + iw,y+fm.ascent()+padding,name);
    painter->drawText(x+padding + iw,y+fm.ascent()+nameBoundingRect.height()+3*padding,date);
}

const QUuid &GraphicsPersonItem::getPersonId() const
{
    return this->person.getId();
}

