#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "calendar_dialog.h"

CalendarDialog::CalendarDialog(const QDate &date, QWidget *parent)
    : QDialog(parent)
    , date(date)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Set date"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout (mainLayout);

    this->calendarWidget = new QCalendarWidget;
    this->calendarWidget->setSelectedDate(date);
    mainLayout->addWidget(this->calendarWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *okButton = new QPushButton(okIcon, tr("OK"));
    buttonBox->addButton(okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

int CalendarDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        this->date = this->calendarWidget->selectedDate();
    }

    return retVal;
}

const QDate &CalendarDialog::getDate() const
{
    return this->date;
}
