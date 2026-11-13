#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalBlocker>

#include "date_edit_widget.h"
#include "calendar_dialog.h"

DateEditWidget::DateEditWidget(const FDate &date, QWidget *parent)
    : QWidget(parent)
    , date(date)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->dayCombo = new QComboBox;
    mainLayout->addWidget(this->dayCombo);

    this->monthCombo = new QComboBox;
    this->monthCombo->addItem(tr("Month"));
    for (int month=1;month<=12;month++)
    {
        this->monthCombo->addItem(QLocale().standaloneMonthName(month));
    }
    mainLayout->addWidget(this->monthCombo,1);

    this->yearSpin = new QSpinBox;
    this->yearSpin->setRange(0,9999);
    this->yearSpin->setSpecialValueText(tr("Year"));
    // Year is applied when it is typed whole, not after each digit.
    this->yearSpin->setKeyboardTracking(false);
    mainLayout->addWidget(this->yearSpin);

    this->fuzzyCheck = new QCheckBox(tr("Approximate"));
    mainLayout->addWidget(this->fuzzyCheck);

    QPushButton *calendarButton = new QPushButton(tr("Calendar"));
    mainLayout->addWidget(calendarButton);

    QPushButton *clearButton = new QPushButton(tr("Clear"));
    mainLayout->addWidget(clearButton);

    this->updateWidgets();

    QObject::connect(this->dayCombo,&QComboBox::activated,this,&DateEditWidget::onValueChanged);
    QObject::connect(this->monthCombo,&QComboBox::activated,this,&DateEditWidget::onValueChanged);
    QObject::connect(this->yearSpin,&QSpinBox::valueChanged,this,&DateEditWidget::onValueChanged);
    QObject::connect(this->fuzzyCheck,&QCheckBox::toggled,this,&DateEditWidget::onFuzzyToggled);
    QObject::connect(calendarButton,&QPushButton::clicked,this,&DateEditWidget::onCalendarButtonClicked);
    QObject::connect(clearButton,&QPushButton::clicked,this,&DateEditWidget::onClearButtonClicked);
}

void DateEditWidget::updateWidgets()
{
    QSignalBlocker yearBlocker(this->yearSpin);
    QSignalBlocker fuzzyBlocker(this->fuzzyCheck);

    FDate::Precision precision = this->date.getPrecision();

    this->yearSpin->setValue(precision >= FDate::Year ? this->date.getYear() : 0);

    this->monthCombo->setCurrentIndex(precision >= FDate::Month ? this->date.getMonth() : 0);
    this->monthCombo->setEnabled(precision >= FDate::Year);

    this->dayCombo->clear();
    this->dayCombo->addItem(tr("Day"));
    if (precision >= FDate::Month)
    {
        int nDays = QDate(this->date.getYear(),this->date.getMonth(),1).daysInMonth();
        for (int day=1;day<=nDays;day++)
        {
            this->dayCombo->addItem(QString::number(day));
        }
    }
    this->dayCombo->setCurrentIndex(precision == FDate::Day ? this->date.getDay() : 0);
    this->dayCombo->setEnabled(precision >= FDate::Month);

    this->fuzzyCheck->setChecked(this->date.getFuzzy());
    this->fuzzyCheck->setEnabled(precision >= FDate::Year);
}

void DateEditWidget::onValueChanged()
{
    int year = this->yearSpin->value();
    int month = this->monthCombo->currentIndex();
    int day = this->dayCombo->currentIndex();

    if (year == 0)
    {
        this->date = FDate();
    }
    else
    {
        if (month == 0)
        {
            day = 0;
        }
        else
        {
            day = qMin(day,QDate(year,month,1).daysInMonth());
        }
        this->date.setYear(year);
        this->date.setMonth(month);
        this->date.setDay(day);
    }

    this->updateWidgets();
    emit this->changed(this->date);
}

void DateEditWidget::onFuzzyToggled(bool checked)
{
    this->date.setFuzzy(checked);
    emit this->changed(this->date);
}

void DateEditWidget::onCalendarButtonClicked()
{
    CalendarDialog calendarDialog(this->date.isEmpty() ? QDate::currentDate() : this->date.toDate(),this);
    int retVal = calendarDialog.exec();
    if (retVal == QDialog::Accepted)
    {
        this->date.setDay(calendarDialog.getDate().day());
        this->date.setMonth(calendarDialog.getDate().month());
        this->date.setYear(calendarDialog.getDate().year());

        this->updateWidgets();
        emit this->changed(this->date);
    }
}

void DateEditWidget::onClearButtonClicked()
{
    this->date = FDate();

    this->updateWidgets();
    emit this->changed(this->date);
}
