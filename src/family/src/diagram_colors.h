#pragma once

#include <QColor>
#include <QGuiApplication>
#include <QStyleHints>

struct DiagramColors
{
    static bool isDarkMode()
    {
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    }

    // === Person Item Colors ===

    static QColor boxBackground()
    {
        return isDarkMode() ? QColor(50, 50, 55) : QColor(252, 252, 254);
    }

    static QColor boxBackgroundBottom()
    {
        return isDarkMode() ? QColor(40, 40, 45) : QColor(245, 245, 248);
    }

    static QColor shadowColor()
    {
        // Dark mode: subtle light glow; Light mode: dark shadow
        return isDarkMode() ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 40);
    }

    static QColor borderColor()
    {
        return isDarkMode() ? QColor(90, 90, 100) : QColor(200, 200, 210);
    }

    static QColor borderColorHover()
    {
        return isDarkMode() ? QColor(130, 160, 220) : QColor(120, 140, 200);
    }

    static QColor borderColorSelected()
    {
        return isDarkMode() ? QColor(100, 140, 230) : QColor(80, 120, 200);
    }

    static QColor textColorPrimary()
    {
        return isDarkMode() ? QColor(235, 235, 240) : QColor(40, 40, 50);
    }

    static QColor textColorSecondary()
    {
        return isDarkMode() ? QColor(170, 170, 185) : QColor(100, 100, 120);
    }

    static QColor highlightOverlay()
    {
        // Dark mode: lighten; Light mode: darken
        return isDarkMode() ? QColor(255, 255, 255, 20) : QColor(0, 0, 0, 25);
    }

    // Header colors by sex - dark mode uses deeper, saturated tones
    static QColor headerMale()
    {
        return isDarkMode() ? QColor(45, 65, 95) : QColor(220, 235, 250);
    }

    static QColor headerMaleBottom()
    {
        return isDarkMode() ? QColor(35, 55, 85) : QColor(200, 220, 245);
    }

    static QColor headerFemale()
    {
        return isDarkMode() ? QColor(90, 55, 70) : QColor(250, 230, 235);
    }

    static QColor headerFemaleBottom()
    {
        return isDarkMode() ? QColor(80, 45, 60) : QColor(245, 215, 225);
    }

    static QColor headerOther()
    {
        return isDarkMode() ? QColor(85, 80, 50) : QColor(250, 248, 225);
    }

    static QColor headerOtherBottom()
    {
        return isDarkMode() ? QColor(75, 70, 40) : QColor(245, 240, 210);
    }

    static QColor headerNeutral()
    {
        return isDarkMode() ? QColor(60, 60, 68) : QColor(235, 235, 240);
    }

    static QColor headerNeutralBottom()
    {
        return isDarkMode() ? QColor(50, 50, 58) : QColor(225, 225, 232);
    }

    static QColor photoBorder()
    {
        return isDarkMode() ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 30);
    }

    // === Relation Item Colors ===

    static QColor relationBoxBackground()
    {
        return isDarkMode() ? QColor(55, 50, 45) : QColor(255, 252, 250);
    }

    static QColor relationBoxBackgroundBottom()
    {
        return isDarkMode() ? QColor(45, 40, 35) : QColor(252, 248, 242);
    }

    static QColor relationShadowColor()
    {
        return isDarkMode() ? QColor(255, 255, 255, 12) : QColor(0, 0, 0, 35);
    }

    static QColor relationBorderColor()
    {
        return isDarkMode() ? QColor(130, 110, 90) : QColor(210, 195, 180);
    }

    static QColor relationBorderColorHover()
    {
        return isDarkMode() ? QColor(180, 150, 120) : QColor(180, 140, 100);
    }

    static QColor relationBorderColorSelected()
    {
        return isDarkMode() ? QColor(200, 140, 90) : QColor(160, 100, 60);
    }

    static QColor relationHeaderTop()
    {
        return isDarkMode() ? QColor(80, 60, 50) : QColor(255, 235, 220);
    }

    static QColor relationHeaderBottom()
    {
        return isDarkMode() ? QColor(70, 50, 40) : QColor(250, 225, 205);
    }

    static QColor relationIconColor()
    {
        return isDarkMode() ? QColor(220, 160, 130) : QColor(200, 120, 100);
    }

    static QColor relationTextPrimary()
    {
        return isDarkMode() ? QColor(235, 230, 220) : QColor(60, 50, 40);
    }

    static QColor relationTextSecondary()
    {
        return isDarkMode() ? QColor(180, 170, 155) : QColor(120, 100, 80);
    }

    // === Edge Item Colors ===

    static QColor edgeLineColor()
    {
        return isDarkMode() ? QColor(130, 130, 145) : QColor(160, 160, 170);
    }

    static QColor edgeLineColorLight()
    {
        return isDarkMode() ? QColor(160, 160, 175) : QColor(190, 190, 200);
    }
};
