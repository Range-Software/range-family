#ifndef PLACE_EDIT_WIDGET_H
#define PLACE_EDIT_WIDGET_H

#include <QWidget>

#include <rfl_place.h>

#include "address_edit_widget.h"
#include "geo_coordinates_edit_widget.h"

class PlaceEditWidget : public QWidget
{

    Q_OBJECT

    protected:

        FPlace place;

        AddressEditWidget *addressEdit;
        GeoCoordinatesEditWidget *geoCoordinatesEdit;

    public:

        explicit PlaceEditWidget(const FPlace &place, QWidget *parent = nullptr);

    protected slots:

        void onAddressChanged(const FAddress &address);
        void onGeoCoordinatesChanged(const FGeoCoordinate &geoCoordinates);

    signals:

        void changed(const FPlace &place);

};

#endif // PLACE_EDIT_WIDGET_H
