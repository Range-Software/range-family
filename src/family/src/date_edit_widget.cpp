#include <QHBoxLayout>

#include "date_edit_widget.h"
#include "calendar_dialog.h"

DateEditWidget::DateEditWidget(const FDate &date, QWidget *parent)
    : QWidget(parent)
    , date(date)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->dateButton = new QPushButton;
    mainLayout->addWidget(this->dateButton,1);

    QPushButton *clearButton = new QPushButton(tr("Clear"));
    this->updateButtonText();
    mainLayout->addWidget(clearButton);

    QObject::connect(this->dateButton,&QPushButton::clicked,this,&DateEditWidget::onDateButtonClicked);
    QObject::connect(clearButton,&QPushButton::clicked,this,&DateEditWidget::onClearButtonClicked);
}

void DateEditWidget::updateButtonText()
{
    this->dateButton->setText(this->date.isEmpty() ? tr("Click here to set date") : QLocale().toString(this->date.toDate()));
}

void DateEditWidget::onDateButtonClicked()
{
    CalendarDialog calendarDialog(this->date.toDate(),this);
    int retVal = calendarDialog.exec();
    if (retVal == QDialog::Accepted)
    {
        this->date.setDay(calendarDialog.getDate().day());
        this->date.setMonth(calendarDialog.getDate().month());
        this->date.setYear(calendarDialog.getDate().year());

        this->updateButtonText();
        emit this->changed(this->date);
    }
}

void DateEditWidget::onClearButtonClicked()
{
    this->date = FDate();

    this->updateButtonText();
    emit this->changed(this->date);
}
