#ifndef DATE_EDIT_WIDGET_H
#define DATE_EDIT_WIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

#include <rfl_date.h>

class DateEditWidget : public QWidget
{

    Q_OBJECT

    protected:

        FDate date;

        //! First item of day and month combo means that the value is not set.
        QComboBox *dayCombo;
        QComboBox *monthCombo;
        //! Zero means that the year is not set.
        QSpinBox *yearSpin;
        QCheckBox *fuzzyCheck;

    public:

        explicit DateEditWidget(const FDate &date, QWidget *parent = nullptr);

    protected:

        //! Show current date in the widgets.
        void updateWidgets();

    protected slots:

        void onValueChanged();
        void onFuzzyToggled(bool checked);
        void onCalendarButtonClicked();
        void onClearButtonClicked();

    signals:

        void changed(const FDate &date);

};

#endif // DATE_EDIT_WIDGET_H
