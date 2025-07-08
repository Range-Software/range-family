#ifndef ADDRESS_EDIT_WIDGET_H
#define ADDRESS_EDIT_WIDGET_H

#include <QWidget>
#include <QLineEdit>

#include <rfl_address.h>

class AddressEditWidget : public QWidget
{
    Q_OBJECT

    protected:

        FAddress address;

        QLineEdit *streetEdit;
        QLineEdit *streetNumberEdit;
        QLineEdit *cityEdit;
        QLineEdit *postalCodeEdit;
        QLineEdit *countryEdit;

    public:

        explicit AddressEditWidget(const FAddress &address, QWidget *parent = nullptr);

    protected slots:

        void onStreetChanged(const QString &text);
        void onStreetNumberChanged(const QString &text);
        void onCityChanged(const QString &text);
        void onPostalCodeChanged(const QString &text);
        void onCountryChanged(const QString &text);

    signals:

        void changed(const FAddress &address);

};

#endif // ADDRESS_EDIT_WIDGET_H
