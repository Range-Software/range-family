#ifndef SCROLL_BAR_WITH_MARKERS_H
#define SCROLL_BAR_WITH_MARKERS_H

#include <QScrollBar>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QGuiApplication>
#include <QStyleHints>

class ScrollBarWithMarkers : public QScrollBar
{
    QList<double> markerPositions;
    QList<double> activeMarkerPositions;

public:

    explicit ScrollBarWithMarkers(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QScrollBar(orientation, parent) {}

    void setMarkers(const QList<double> &positions)
    {
        markerPositions = positions;
        update();
    }

    void setActiveMarkers(const QList<double> &positions)
    {
        activeMarkerPositions = positions;
        update();
    }

protected:

    void paintEvent(QPaintEvent *event) override
    {
        QScrollBar::paintEvent(event);

        QPainter painter(this);
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect groove = style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, this);

        bool isDark = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
        QColor lineColor = isDark ? QColor(255, 215, 0) : QColor(200, 140, 0);
        lineColor.setAlpha(200);

        for (double pos : markerPositions)
        {
            int y = groove.top() + static_cast<int>(pos * groove.height());
            painter.fillRect(groove.left(), y, groove.width(), 2, lineColor);
        }

        QColor dotColor = isDark ? QColor(100, 180, 255) : QColor(0, 100, 200);
        dotColor.setAlpha(220);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(dotColor);
        painter.setPen(Qt::NoPen);

        int dotSize = qMin(groove.width(), 8);
        int dotX = groove.left() + (groove.width() - dotSize) / 2;
        for (double pos : activeMarkerPositions)
        {
            int y = groove.top() + static_cast<int>(pos * groove.height());
            painter.drawEllipse(dotX, y - dotSize / 2, dotSize, dotSize);
        }
    }
};

#endif // SCROLL_BAR_WITH_MARKERS_H
