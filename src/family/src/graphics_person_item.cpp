#include <QPainter>
#include <QFontMetrics>
#include <QStyleOptionGraphicsItem>
#include <QLinearGradient>

#include "graphics_person_item.h"
#include "diagram_colors.h"

namespace {
    // Styling constants
    constexpr qreal rounding = 10.0;
    constexpr qreal padding = 8.0;
    constexpr qreal shadowOffset = 3.0;
    constexpr qreal shadowBlur = 6.0;
    constexpr qreal borderWidth = 1.5;
    constexpr qreal hoverBorderWidth = 2.5;
    constexpr qreal selectedBorderWidth = 3.0;
}

GraphicsPersonItem::GraphicsPersonItem(const FPerson &person, const QRectF &slot, GraphicsNodeItem *parent)
    : GraphicsNodeItem(slot,parent)
    , person(person)
{

}

void GraphicsPersonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    QFont baseFont = painter->font();
    QFont nameFont = baseFont;
    nameFont.setBold(true);
    nameFont.setPointSizeF(baseFont.pointSizeF() * 1.05);

    QFont dateFont = baseFont;
    dateFont.setPointSizeF(baseFont.pointSizeF() * 0.9);

    QFontMetrics fmName(nameFont);
    QFontMetrics fmDate(dateFont);
    QImage personImage;

    QString name = this->person.getName().toString();
    QString date = QLocale().toString(this->person.getBirth().getDate().toDate(),QLocale::ShortFormat)
                 + " – "
                 + QLocale().toString(this->person.getDeath().getDate().toDate(),QLocale::ShortFormat);

    QRect nameBoundingRect = fmName.boundingRect(name);
    QRect dateBoundingRect = fmDate.boundingRect(date);

    qreal textWidth = std::max(nameBoundingRect.width(), dateBoundingRect.width());
    qreal headerHeight = nameBoundingRect.height() + 2 * padding;
    qreal w = textWidth + 2 * padding;
    qreal h = headerHeight + dateBoundingRect.height() + 2 * padding;

    qreal imageSize = 0;
    qreal imagePadding = 0;

    if (!this->person.getPicture().getData().isEmpty())
    {
        imageSize = h - 2 * padding;
        imagePadding = padding;
        QImage sourceImage = QImage::fromData(this->person.getPicture().getData())
                             .scaled(imageSize, imageSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        // Crop to square if needed
        if (sourceImage.width() != sourceImage.height())
        {
            int size = std::min(sourceImage.width(), sourceImage.height());
            sourceImage = sourceImage.copy((sourceImage.width() - size) / 2,
                                           (sourceImage.height() - size) / 2,
                                           size, size);
        }

        // Create circular image
        personImage = QImage(sourceImage.size(), QImage::Format_ARGB32);
        personImage.fill(Qt::transparent);

        QPainter p(&personImage);
        p.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath clipPath;
        clipPath.addEllipse(sourceImage.rect());
        p.setClipPath(clipPath);
        p.drawImage(0, 0, sourceImage);
    }

    w += imageSize + (imageSize > 0 ? imagePadding : 0);

    qreal x = this->slot.left() + (this->slot.width() - w) / 2.0;
    qreal y = this->slot.top() + (this->slot.height() - h) / 2.0;

    // Expand bounding rect to include shadow
    this->setBoundingRect(QRectF(x - shadowBlur, y - shadowBlur,
                                  w + 2 * shadowBlur + shadowOffset,
                                  h + 2 * shadowBlur + shadowOffset));

    QRectF boxRect(x, y, w, h);
    QRectF headerRect(x, y, w, headerHeight);

    // Determine header colors based on sex
    QColor headerTop = DiagramColors::headerNeutral();
    QColor headerBottom = DiagramColors::headerNeutralBottom();

    if (this->person.getSex().toLower() == FPerson::SexDesc::male.toLower())
    {
        headerTop = DiagramColors::headerMale();
        headerBottom = DiagramColors::headerMaleBottom();
    }
    else if (this->person.getSex().toLower() == FPerson::SexDesc::female.toLower())
    {
        headerTop = DiagramColors::headerFemale();
        headerBottom = DiagramColors::headerFemaleBottom();
    }
    else if (this->person.getSex().toLower() == FPerson::SexDesc::other.toLower())
    {
        headerTop = DiagramColors::headerOther();
        headerBottom = DiagramColors::headerOtherBottom();
    }

    // Draw shadow
    QColor shadow = DiagramColors::shadowColor();
    painter->setPen(Qt::NoPen);
    for (int i = shadowBlur; i > 0; i -= 2)
    {
        QColor sc = shadow;
        sc.setAlpha(shadow.alpha() * (shadowBlur - i) / shadowBlur);
        painter->setBrush(sc);
        painter->drawRoundedRect(boxRect.adjusted(-i + shadowOffset, -i + shadowOffset,
                                                   i + shadowOffset, i + shadowOffset),
                                  rounding + i/2, rounding + i/2);
    }

    // Draw main box with gradient
    QLinearGradient boxGradient(boxRect.topLeft(), boxRect.bottomLeft());
    boxGradient.setColorAt(0.0, DiagramColors::boxBackground());
    boxGradient.setColorAt(1.0, DiagramColors::boxBackgroundBottom());

    painter->setBrush(boxGradient);

    // Determine border style based on state
    QColor currentBorderColor = DiagramColors::borderColor();
    qreal currentBorderWidth = borderWidth;

    if (this->isSelected())
    {
        currentBorderColor = DiagramColors::borderColorSelected();
        currentBorderWidth = selectedBorderWidth;
    }
    else if (this->isUnderMouse())
    {
        currentBorderColor = DiagramColors::borderColorHover();
        currentBorderWidth = hoverBorderWidth;
    }

    painter->setPen(QPen(currentBorderColor, currentBorderWidth));
    painter->drawRoundedRect(boxRect, rounding, rounding);

    // Draw header with gradient
    QLinearGradient headerGradient(headerRect.topLeft(), headerRect.bottomLeft());
    headerGradient.setColorAt(0.0, headerTop);
    headerGradient.setColorAt(1.0, headerBottom);

    painter->setPen(Qt::NoPen);
    painter->setBrush(headerGradient);

    // Clip to top rounded corners only
    QPainterPath headerPath;
    headerPath.setFillRule(Qt::WindingFill);
    headerPath.addRoundedRect(headerRect, rounding, rounding);
    // Add rectangle to square off bottom corners
    headerPath.addRect(QRectF(x, y + headerHeight - rounding, w, rounding));

    painter->drawPath(headerPath.simplified());

    // Draw highlight overlay if highlighted
    if (this->highlighted)
    {
        painter->setBrush(DiagramColors::highlightOverlay());
        painter->drawRoundedRect(boxRect, rounding, rounding);
    }

    // Draw circular profile picture
    if (!personImage.isNull())
    {
        qreal imgX = x + padding;
        qreal imgY = y + padding;
        qreal imgSize = personImage.width();

        painter->drawImage(QPointF(imgX, imgY), personImage);

        // Draw subtle border around photo
        painter->setPen(QPen(DiagramColors::photoBorder(), 1.5));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QRectF(imgX, imgY, imgSize, imgSize));
    }

    // Draw text
    qreal textX = x + padding + imageSize + (imageSize > 0 ? imagePadding : 0);

    // Name (bold)
    painter->setFont(nameFont);
    painter->setPen(DiagramColors::textColorPrimary());
    painter->drawText(textX, y + fmName.ascent() + padding, name);

    // Date (smaller, lighter)
    painter->setFont(dateFont);
    painter->setPen(DiagramColors::textColorSecondary());
    painter->drawText(textX, y + headerHeight + fmDate.ascent() + padding, date);

    // Restore font
    painter->setFont(baseFont);
}

const QUuid &GraphicsPersonItem::getPersonId() const
{
    return this->person.getId();
}

