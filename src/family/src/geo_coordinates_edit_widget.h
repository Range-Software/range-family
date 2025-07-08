#ifndef GEO_COORDINATES_EDIT_WIDGET_H
#define GEO_COORDINATES_EDIT_WIDGET_H

#include <QWidget>
#include <QLineEdit>

#include <rfl_geo_coordinate.h>

class GeoCoordinatesEditWidget : public QWidget
{
    Q_OBJECT

    protected:

        FGeoCoordinate geoCoordinates;

        QLineEdit *latitudeEdit;
        QLineEdit *longitudeEdit;
        QLineEdit *altitudeEdit;

    public:

        explicit GeoCoordinatesEditWidget(const FGeoCoordinate &geoCoordinates, QWidget *parent = nullptr);

    protected slots:

        void onLatitudeChanged(const QString &text);
        void onLongitudeChanged(const QString &text);
        void onAltitudeChanged(const QString &text);
        void onClearButtonClicked();

    signals:

        void changed(const FGeoCoordinate &geoCoordinates);

};

#endif // GEO_COORDINATES_EDIT_WIDGET_H
