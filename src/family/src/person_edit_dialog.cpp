#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include "person_edit_dialog.h"

PersonEditDialog::PersonEditDialog(FTree *familyTree, const FPerson &person, QWidget *parent)
    : QDialog(parent)
    , familyTree(familyTree)
    , person(person)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon editIcon(":/icons/action/pixmaps/range-edit.svg");

    this->setWindowTitle(tr("Edit person"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->personEditWidget = new PersonEditWidget(this->person);
    mainLayout->addWidget(this->personEditWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->editButton = new QPushButton(editIcon, tr("Edit"));
    this->editButton->setEnabled(false);
    buttonBox->addButton(this->editButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(this->personEditWidget,&PersonEditWidget::changed,this,&PersonEditDialog::onPersonEdidWidgetChanged);
}

int PersonEditDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        FPerson person = this->personEditWidget->getPerson();
        RToolInput toolInput;
        toolInput.addAction(FToolAction::modifyPerson(this->familyTree,person));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }

    return retVal;
}

void PersonEditDialog::onPersonEdidWidgetChanged(const FPerson &)
{
    this->editButton->setEnabled(true);
    this->editButton->setDefault(true);
}
