#include <QHBoxLayout>
#include <QApplication>

#include <rfl_address.h>

#include "address_edit_widget.h"

#define _FA_TR(qstr) qApp->translate("FAddress",qstr.toUtf8().constData())

AddressEditWidget::AddressEditWidget(const FAddress &address, QWidget *parent)
    : QWidget(parent)
    , address(address)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->streetEdit = new QLineEdit(address.getStreet());
    this->streetEdit->setPlaceholderText(_FA_TR(FAddress::ElementDesc::street));
    this->streetEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(this->streetEdit);

    this->streetNumberEdit = new QLineEdit(address.getStreetNumber());
    this->streetNumberEdit->setPlaceholderText(_FA_TR(FAddress::ElementDesc::streetNumber));
    this->streetNumberEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(this->streetNumberEdit);

    this->cityEdit = new QLineEdit(address.getCity());
    this->cityEdit->setPlaceholderText(_FA_TR(FAddress::ElementDesc::city));
    this->cityEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(this->cityEdit);

    this->postalCodeEdit = new QLineEdit(address.getPostalCode());
    this->postalCodeEdit->setPlaceholderText(_FA_TR(FAddress::ElementDesc::postalCode));
    this->postalCodeEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(this->postalCodeEdit);

    this->countryEdit = new QLineEdit(address.getCountry());
    this->countryEdit->setPlaceholderText(_FA_TR(FAddress::ElementDesc::country));
    this->countryEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(this->countryEdit);

    QObject::connect(this->streetEdit,&QLineEdit::textChanged,this,&AddressEditWidget::onStreetChanged);
    QObject::connect(this->streetNumberEdit,&QLineEdit::textChanged,this,&AddressEditWidget::onStreetNumberChanged);
    QObject::connect(this->cityEdit,&QLineEdit::textChanged,this,&AddressEditWidget::onCityChanged);
    QObject::connect(this->postalCodeEdit,&QLineEdit::textChanged,this,&AddressEditWidget::onPostalCodeChanged);
    QObject::connect(this->countryEdit,&QLineEdit::textChanged,this,&AddressEditWidget::onCountryChanged);
}

void AddressEditWidget::onStreetChanged(const QString &text)
{
    this->address.setStreet(text);
    emit this->changed(this->address);
}

void AddressEditWidget::onStreetNumberChanged(const QString &text)
{
    this->address.setStreetNumber(text);
    emit this->changed(this->address);
}

void AddressEditWidget::onCityChanged(const QString &text)
{
    this->address.setCity(text);
    emit this->changed(this->address);
}

void AddressEditWidget::onPostalCodeChanged(const QString &text)
{
    this->address.setPostalCode(text);
    emit this->changed(this->address);
}

void AddressEditWidget::onCountryChanged(const QString &text)
{
    this->address.setCountry(text);
    emit this->changed(this->address);
}
