#include <QGridLayout>
#include <QLabel>
#include <QApplication>

#include "place_edit_widget.h"

#define _FP_TR(qstr) qApp->translate("FPlace",qstr.toUtf8().constData())

PlaceEditWidget::PlaceEditWidget(const FPlace &place, QWidget *parent)
    : QWidget(parent)
    , place(place)
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setColumnStretch(1,1);
    this->setLayout(mainLayout);

    QLabel *addressLabel = new QLabel(_FP_TR(FPlace::ElementDesc::address));
    mainLayout->addWidget(addressLabel,0,0);

    this->addressEdit = new AddressEditWidget(this->place.getAddress());
    mainLayout->addWidget(addressEdit,0,1);

    QLabel *geoCoordinatesLabel = new QLabel(_FP_TR(FPlace::ElementDesc::geoCoordinate));
    mainLayout->addWidget(geoCoordinatesLabel,1,0);

    this->geoCoordinatesEdit = new GeoCoordinatesEditWidget(this->place.getGeoCoordinates());
    mainLayout->addWidget(geoCoordinatesEdit,1,1);

    QObject::connect(this->addressEdit,&AddressEditWidget::changed,this,&PlaceEditWidget::onAddressChanged);
    QObject::connect(this->geoCoordinatesEdit,&GeoCoordinatesEditWidget::changed,this,&PlaceEditWidget::onGeoCoordinatesChanged);
}

void PlaceEditWidget::onAddressChanged(const FAddress &address)
{
    this->place.setAddress(address);
    emit this->changed(this->place);
}

void PlaceEditWidget::onGeoCoordinatesChanged(const FGeoCoordinate &geoCoordinates)
{
    this->place.setGeoCoordinates(geoCoordinates);
    emit this->changed(this->place);
}
