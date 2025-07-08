#ifndef DATE_EDIT_WIDGET_H
#define DATE_EDIT_WIDGET_H

#include <QWidget>
#include <QPushButton>

#include <rfl_date.h>

class DateEditWidget : public QWidget
{

    Q_OBJECT

    protected:

        FDate date;

        QPushButton *dateButton;

    public:

        explicit DateEditWidget(const FDate &date, QWidget *parent = nullptr);

    protected:

        void updateButtonText();

    protected slots:

        void onDateButtonClicked();
        void onClearButtonClicked();

    signals:

        void changed(const FDate &date);

};

#endif // DATE_EDIT_WIDGET_H
