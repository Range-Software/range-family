#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QRegularExpression>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include <rgl_icon.h>

#include "person_suggested_dates_dialog.h"

//! Date in long format of current locale without the day of week.
static QString dateToString(const FDate &date)
{
    if (date.getPrecision() != FDate::Day)
    {
        return date.toLocaleString();
    }
    static const QRegularExpression dayOfWeek("^dddd[,\\s]*|[,\\s]*dddd");
    QString format(QLocale().dateFormat(QLocale::LongFormat));
    return QLocale().toString(date.toDate(),format.remove(dayOfWeek));
}

PersonSuggestedDatesDialog::PersonSuggestedDatesDialog(FTree *familyTree, const FPerson &rPerson, QWidget *parent)
    : QDialog(parent)
    , familyTree(familyTree)
    , person(rPerson)
    , birthCheck(nullptr)
    , deathCheck(nullptr)
{
    QIcon cancelIcon = RIcon::fromFile(":/icons/action/pixmaps/range-cancel.svg");
    QIcon editIcon = RIcon::fromFile(":/icons/action/pixmaps/range-edit.svg");

    this->setWindowTitle(tr("Suggested person birth and death dates"));
    this->setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    this->setLayout(mainLayout);

    this->familyTree->findPersonBirthAndDeathDates(this->person.getId(),this->birth,this->death);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    formLayout->addRow(new QLabel("<b>" + rPerson.getName().toString() + "</b>"));
    this->birthCheck = this->addEstimateRow(formLayout,tr("Birth"),this->birth,this->person.getBirth().getDate());
    this->deathCheck = this->addEstimateRow(formLayout,tr("Death"),this->death,this->person.getDeath().getDate());

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *editButton = new QPushButton(editIcon, tr("Edit"));
    buttonBox->addButton(editButton,QDialogButtonBox::AcceptRole);

    auto updateEditButton = [this,editButton]()
    {
        editButton->setEnabled((this->birthCheck && this->birthCheck->isChecked()) ||
                               (this->deathCheck && this->deathCheck->isChecked()));
    };
    for (QCheckBox *checkBox : { this->birthCheck, this->deathCheck })
    {
        if (checkBox)
        {
            QObject::connect(checkBox,&QCheckBox::toggled,this,updateEditButton);
        }
    }
    updateEditButton();

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

QCheckBox *PersonSuggestedDatesDialog::addEstimateRow(QFormLayout *formLayout, const QString &label, const FDateEstimate &estimate, const FDate &recordedDate)
{
    if (estimate.status != FDateEstimate::Estimated)
    {
        QString text(tr("unknown"));
        if (estimate.status == FDateEstimate::Known)
        {
            text = dateToString(estimate.date);
        }
        else if (estimate.status == FDateEstimate::Conflict)
        {
            text = tr("dates of relatives contradict each other");
        }
        formLayout->addRow(label + ":",new QLabel(text));
        return nullptr;
    }

    QStringList details;
    if (!estimate.low.isEmpty() && !estimate.high.isEmpty())
    {
        details.append(tr("between %1 and %2").arg(estimate.low.getYear()).arg(estimate.high.getYear()));
    }
    else if (!estimate.low.isEmpty())
    {
        details.append(tr("not before %1").arg(estimate.low.getYear()));
    }
    else if (!estimate.high.isEmpty())
    {
        details.append(tr("not after %1").arg(estimate.high.getYear()));
    }
    if (!recordedDate.isEmpty())
    {
        details.append(tr("replaces %1").arg(dateToString(recordedDate)));
    }

    QCheckBox *checkBox = new QCheckBox(tr("about %1").arg(dateToString(estimate.date)));
    checkBox->setChecked(true);

    QVBoxLayout *rowLayout = new QVBoxLayout;
    rowLayout->addWidget(checkBox);
    if (!details.isEmpty())
    {
        QLabel *detailsLabel = new QLabel(details.join(", "));
        detailsLabel->setEnabled(false);
        rowLayout->addWidget(detailsLabel);
    }
    formLayout->addRow(label + ":",rowLayout);

    return checkBox;
}

int PersonSuggestedDatesDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        if (this->birthCheck && this->birthCheck->isChecked())
        {
            FEvent birthEvent = this->person.getBirth();
            birthEvent.setDate(this->birth.date);
            this->person.setBirth(birthEvent);
        }
        if (this->deathCheck && this->deathCheck->isChecked())
        {
            FEvent deathEvent = this->person.getDeath();
            deathEvent.setDate(this->death.date);
            this->person.setDeath(deathEvent);
        }

        RToolInput toolInput;
        toolInput.addAction(FToolAction::modifyPerson(this->familyTree,this->person));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }

    return retVal;
}
