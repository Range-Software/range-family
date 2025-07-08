#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include "person_add_dialog.h"

PersonAddDialog::PersonAddDialog(FTree *familyTree, const FPerson &newPerson, QWidget *parent)
    : QDialog(parent)
    , familyTree(familyTree)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Add person"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->personEditWidget = new PersonEditWidget(newPerson);
    mainLayout->addWidget(this->personEditWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *addButton = new QPushButton(addIcon, tr("Add"));
    buttonBox->addButton(addButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

int PersonAddDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        RToolInput toolInput;
        FPerson person = this->personEditWidget->getPerson();
        if (person.getId().isNull())
        {
            person.setId(FTree::generateId());
        }
        this->newPersonId = person.getId();
        toolInput.addAction(FToolAction::addPerson(this->familyTree,person));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }

    return retVal;
}

const QUuid &PersonAddDialog::getNewPersonId() const
{
    return this->newPersonId;
}
