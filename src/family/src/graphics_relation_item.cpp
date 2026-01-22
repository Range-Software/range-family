#include <QPainter>
#include <QFontMetrics>
#include <QLinearGradient>

#include "graphics_relation_item.h"

namespace {
    // Styling constants
    constexpr qreal rounding = 12.0;
    constexpr qreal padding = 8.0;
    constexpr qreal shadowOffset = 3.0;
    constexpr qreal shadowBlur = 6.0;
    constexpr qreal borderWidth = 1.5;
    constexpr qreal hoverBorderWidth = 2.5;
    constexpr qreal selectedBorderWidth = 3.0;
    constexpr qreal iconSize = 16.0;
    constexpr qreal iconPadding = 6.0;

    // Colors - warm tones to distinguish from person nodes
    const QColor boxBackground(255, 252, 250);
    const QColor boxBackgroundBottom(252, 248, 242);
    const QColor shadowColor(0, 0, 0, 35);
    const QColor borderColor(210, 195, 180);
    const QColor borderColorHover(180, 140, 100);
    const QColor borderColorSelected(160, 100, 60);
    const QColor textColorPrimary(60, 50, 40);
    const QColor textColorSecondary(120, 100, 80);

    // Header colors - warm coral/peach tones
    const QColor headerTop(255, 235, 220);
    const QColor headerBottom(250, 225, 205);

    // Icon color
    const QColor iconColor(200, 120, 100);

    // Highlighted overlay
    const QColor highlightOverlay(0, 0, 0, 20);
}

GraphicsRelationItem::GraphicsRelationItem(const FRelation &relation, const QRectF &slot, GraphicsNodeItem *parent)
    : GraphicsNodeItem(slot,parent)
    , relation(relation)
{

}

void GraphicsRelationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QFont baseFont = painter->font();
    QFont nameFont = baseFont;
    nameFont.setBold(true);
    nameFont.setPointSizeF(baseFont.pointSizeF() * 1.0);

    QFont dateFont = baseFont;
    dateFont.setPointSizeF(baseFont.pointSizeF() * 0.9);

    QFontMetrics fmName(nameFont);
    QFontMetrics fmDate(dateFont);

    QString name = QObject::tr("Relation") + " (" + QObject::tr("children") + ": " + QLocale().toString(this->relation.getChildren().size()) + ")";
    QString date = QLocale().toString(this->relation.getBegin().getDate().toDate(),QLocale::ShortFormat)
                   + " – "
                   + QLocale().toString(this->relation.getEnd().getDate().toDate(),QLocale::ShortFormat);

    QRect nameBoundingRect = fmName.boundingRect(name);
    QRect dateBoundingRect = fmDate.boundingRect(date);

    qreal textWidth = std::max(nameBoundingRect.width(), dateBoundingRect.width());
    qreal headerHeight = nameBoundingRect.height() + 2 * padding;
    qreal w = textWidth + iconSize + iconPadding + 2 * padding;
    qreal h = headerHeight + dateBoundingRect.height() + 2 * padding;
    qreal x = this->slot.left() + (this->slot.width() - w) / 2.0;
    qreal y = this->slot.top() + (this->slot.height() - h) / 2.0;

    // Expand bounding rect to include shadow
    this->setBoundingRect(QRectF(x - shadowBlur, y - shadowBlur,
                                  w + 2 * shadowBlur + shadowOffset,
                                  h + 2 * shadowBlur + shadowOffset));

    QRectF boxRect(x, y, w, h);
    QRectF headerRect(x, y, w, headerHeight);

    // Draw shadow
    painter->setPen(Qt::NoPen);
    for (int i = shadowBlur; i > 0; i -= 2)
    {
        QColor sc = shadowColor;
        sc.setAlpha(shadowColor.alpha() * (shadowBlur - i) / shadowBlur);
        painter->setBrush(sc);
        painter->drawRoundedRect(boxRect.adjusted(-i + shadowOffset, -i + shadowOffset,
                                                   i + shadowOffset, i + shadowOffset),
                                  rounding + i/2, rounding + i/2);
    }

    // Draw main box with gradient
    QLinearGradient boxGradient(boxRect.topLeft(), boxRect.bottomLeft());
    boxGradient.setColorAt(0.0, boxBackground);
    boxGradient.setColorAt(1.0, boxBackgroundBottom);

    painter->setBrush(boxGradient);

    // Determine border style based on state
    QColor currentBorderColor = borderColor;
    qreal currentBorderWidth = borderWidth;

    if (this->isSelected())
    {
        currentBorderColor = borderColorSelected;
        currentBorderWidth = selectedBorderWidth;
    }
    else if (this->isUnderMouse())
    {
        currentBorderColor = borderColorHover;
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
    headerPath.addRect(QRectF(x, y + headerHeight - rounding, w, rounding));

    painter->drawPath(headerPath.simplified());

    // Draw highlight overlay if highlighted
    if (this->highlighted)
    {
        painter->setBrush(highlightOverlay);
        painter->drawRoundedRect(boxRect, rounding, rounding);
    }

    // Draw link icon (two interlocking rings)
    qreal iconX = x + padding;
    qreal iconY = y + (headerHeight - iconSize) / 2.0;

    painter->setPen(QPen(iconColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(Qt::NoBrush);

    // Draw two interlocking circles
    qreal ringSize = iconSize * 0.6;
    qreal ringOffset = iconSize * 0.25;
    painter->drawEllipse(QRectF(iconX, iconY + (iconSize - ringSize) / 2.0, ringSize, ringSize));
    painter->drawEllipse(QRectF(iconX + ringOffset, iconY + (iconSize - ringSize) / 2.0, ringSize, ringSize));

    // Draw text
    qreal textX = x + padding + iconSize + iconPadding;

    // Name (bold)
    painter->setFont(nameFont);
    painter->setPen(textColorPrimary);
    painter->drawText(textX, y + fmName.ascent() + padding, name);

    // Date (smaller, lighter)
    painter->setFont(dateFont);
    painter->setPen(textColorSecondary);
    painter->drawText(textX, y + headerHeight + fmDate.ascent() + padding, date);

    // Restore font
    painter->setFont(baseFont);
}

const QUuid &GraphicsRelationItem::getRelationId() const
{
    return this->relation.getId();
}

