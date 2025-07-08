#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>

#include "geo_coordinates_edit_widget.h"

#define _FG_TR(qstr) qApp->translate("FGeoCoordinate",qstr.toUtf8().constData())

GeoCoordinatesEditWidget::GeoCoordinatesEditWidget(const FGeoCoordinate &geoCoordinates, QWidget *parent)
    : QWidget(parent)
    , geoCoordinates(geoCoordinates)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->latitudeEdit = new QLineEdit;
    this->latitudeEdit->setPlaceholderText(_FG_TR(FGeoCoordinate::ElementDesc::latitude));
    mainLayout->addWidget(this->latitudeEdit);

    this->longitudeEdit = new QLineEdit;
    this->longitudeEdit->setPlaceholderText(_FG_TR(FGeoCoordinate::ElementDesc::longitude));
    mainLayout->addWidget(this->longitudeEdit);

    this->altitudeEdit = new QLineEdit;
    this->altitudeEdit->setPlaceholderText(_FG_TR(FGeoCoordinate::ElementDesc::altitude));
    mainLayout->addWidget(this->altitudeEdit);

    if (!geoCoordinates.isEmpty())
    {
        this->latitudeEdit->setText(QLocale().toString(geoCoordinates.getLatitude()));
        this->longitudeEdit->setText(QLocale().toString(geoCoordinates.getLongitude()));
        this->altitudeEdit->setText(QLocale().toString(geoCoordinates.getAltitude()));
    }

    QPushButton *clearButton = new QPushButton(GeoCoordinatesEditWidget::tr("Clear"));
    mainLayout->addWidget(clearButton);

    QObject::connect(this->latitudeEdit,&QLineEdit::textChanged,this,&GeoCoordinatesEditWidget::onLatitudeChanged);
    QObject::connect(this->longitudeEdit,&QLineEdit::textChanged,this,&GeoCoordinatesEditWidget::onLongitudeChanged);
    QObject::connect(this->altitudeEdit,&QLineEdit::textChanged,this,&GeoCoordinatesEditWidget::onAltitudeChanged);
    QObject::connect(clearButton,&QPushButton::clicked,this,&GeoCoordinatesEditWidget::onClearButtonClicked);
}

void GeoCoordinatesEditWidget::onLatitudeChanged(const QString &text)
{
    this->geoCoordinates.setLatitude(QLocale().toDouble(text));
    emit this->changed(this->geoCoordinates);
}

void GeoCoordinatesEditWidget::onLongitudeChanged(const QString &text)
{
    this->geoCoordinates.setLongitude(QLocale().toDouble(text));
    emit this->changed(this->geoCoordinates);
}

void GeoCoordinatesEditWidget::onAltitudeChanged(const QString &text)
{
    this->geoCoordinates.setAltitude(QLocale().toDouble(text));
    emit this->changed(this->geoCoordinates);
}

void GeoCoordinatesEditWidget::onClearButtonClicked()
{
    this->latitudeEdit->clear();
    this->longitudeEdit->clear();
    this->altitudeEdit->clear();
}
