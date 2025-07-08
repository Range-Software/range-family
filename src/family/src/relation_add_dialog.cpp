#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include "relation_add_dialog.h"

RelationAddDialog::RelationAddDialog(FTree *familyTree, const FRelation &newRelation, QWidget *parent)
    : QDialog(parent)
    , familyTree(familyTree)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Add relation"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->relationEditWidget = new RelationEditWidget(this->familyTree,newRelation);
    mainLayout->addWidget(this->relationEditWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *addButton = new QPushButton(addIcon, tr("Add"));
    buttonBox->addButton(addButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

int RelationAddDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        FRelation relation = this->relationEditWidget->getRelation();
        if (relation.getId().isNull())
        {
            relation.setId(FTree::generateId());
        }
        this->newRelationId = relation.getId();
        RToolInput toolInput;
        toolInput.addAction(FToolAction::addRelation(this->familyTree,relation));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }

    return retVal;
}

const QUuid &RelationAddDialog::getNewRelationId() const
{
    return this->newRelationId;
}
