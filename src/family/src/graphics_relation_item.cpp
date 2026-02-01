#include <QPainter>
#include <QFontMetrics>
#include <QLinearGradient>

#include "graphics_relation_item.h"
#include "diagram_colors.h"

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
    QColor shadow = DiagramColors::relationShadowColor();
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
    boxGradient.setColorAt(0.0, DiagramColors::relationBoxBackground());
    boxGradient.setColorAt(1.0, DiagramColors::relationBoxBackgroundBottom());

    painter->setBrush(boxGradient);

    // Determine border style based on state
    QColor currentBorderColor = DiagramColors::relationBorderColor();
    qreal currentBorderWidth = borderWidth;

    if (this->isSelected())
    {
        currentBorderColor = DiagramColors::relationBorderColorSelected();
        currentBorderWidth = selectedBorderWidth;
    }
    else if (this->isUnderMouse())
    {
        currentBorderColor = DiagramColors::relationBorderColorHover();
        currentBorderWidth = hoverBorderWidth;
    }

    painter->setPen(QPen(currentBorderColor, currentBorderWidth));
    painter->drawRoundedRect(boxRect, rounding, rounding);

    // Draw header with gradient
    QLinearGradient headerGradient(headerRect.topLeft(), headerRect.bottomLeft());
    headerGradient.setColorAt(0.0, DiagramColors::relationHeaderTop());
    headerGradient.setColorAt(1.0, DiagramColors::relationHeaderBottom());

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
        painter->setBrush(DiagramColors::highlightOverlay());
        painter->drawRoundedRect(boxRect, rounding, rounding);
    }

    // Draw link icon (two interlocking rings)
    qreal iconX = x + padding;
    qreal iconY = y + (headerHeight - iconSize) / 2.0;

    painter->setPen(QPen(DiagramColors::relationIconColor(), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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
    painter->setPen(DiagramColors::relationTextPrimary());
    painter->drawText(textX, y + fmName.ascent() + padding, name);

    // Date (smaller, lighter)
    painter->setFont(dateFont);
    painter->setPen(DiagramColors::relationTextSecondary());
    painter->drawText(textX, y + headerHeight + fmDate.ascent() + padding, date);

    // Restore font
    painter->setFont(baseFont);
}

const QUuid &GraphicsRelationItem::getRelationId() const
{
    return this->relation.getId();
}

