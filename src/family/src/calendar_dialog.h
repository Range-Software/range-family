#ifndef CALENDAR_DIALOG_H
#define CALENDAR_DIALOG_H

#include <QDialog>
#include <QDate>
#include <QCalendarWidget>

class CalendarDialog : public QDialog
{
    Q_OBJECT

    protected:

        QDate date;

        QCalendarWidget *calendarWidget;

    public:

        //! Constructor.
        explicit CalendarDialog(const QDate &date, QWidget *parent = nullptr);

        //! Execute dialog.
        int exec();

        //! Return reference to current date.
        const QDate &getDate() const;

    signals:

};

#endif // CALENDAR_DIALOG_H
