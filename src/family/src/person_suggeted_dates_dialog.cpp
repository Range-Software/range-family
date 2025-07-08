#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include "person_suggeted_dates_dialog.h"

PersonSuggetedDatesDialog::PersonSuggetedDatesDialog(FTree *familyTree, const FPerson &rPerson, QWidget *parent)
    : QDialog(parent)
    , familyTree(familyTree)
    , person(rPerson)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon editIcon(":/icons/action/pixmaps/range-edit.svg");

    this->setWindowTitle(tr("Suggested person birth and death dates"));
    this->setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    this->setLayout(mainLayout);

    FDate birthDate, deathDate;
    this->familyTree->findPersonBirthAndDeathDates(this->person.getId(),birthDate,deathDate);

    QString birthText(tr("unknown"));
    QString deathText(tr("unknown"));

    if (!birthDate.isEmpty() && birthDate.isValid())
    {
        birthText = QLocale().toString(birthDate.toDate());
        if (birthDate.getFuzzy())
        {
            birthText = "<i>" + birthText + "</i>";
        }

        FEvent birthEvent = this->person.getBirth();
        birthEvent.setDate(birthDate);
        this->person.setBirth(birthEvent);
    }
    if (!deathDate.isEmpty() && deathDate.isValid())
    {
        deathText = QLocale().toString(deathDate.toDate());
        if (deathDate.getFuzzy())
        {
            deathText = "<i>" + deathText + "</i>";
        }

        FEvent deathEvent = this->person.getDeath();
        deathEvent.setDate(deathDate);
        this->person.setDeath(deathEvent);
    }

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    formLayout->addRow(new QLabel("<b>" + rPerson.getName().toString() + "</b>"));
    formLayout->addRow(tr("Birth") + ":",new QLabel(birthText));
    formLayout->addRow(tr("Death") + ":",new QLabel(deathText));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *editButton = new QPushButton(editIcon, tr("Edit"));
    buttonBox->addButton(editButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

int PersonSuggetedDatesDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        RToolInput toolInput;
        toolInput.addAction(FToolAction::modifyPerson(this->familyTree,this->person));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }

    return retVal;
}
