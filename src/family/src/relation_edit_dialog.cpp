#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include "relation_edit_dialog.h"

RelationEditDialog::RelationEditDialog(FTree *familyTree, const FRelation &relation, QWidget *parent)
    : QDialog(parent)
    , familyTree(familyTree)
    , relation(relation)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon editIcon(":/icons/action/pixmaps/range-edit.svg");

    this->setWindowTitle(tr("Edit relation"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->relationEditWidget = new RelationEditWidget(this->familyTree,this->relation);
    mainLayout->addWidget(this->relationEditWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->editButton = new QPushButton(editIcon, tr("Edit"));
    this->editButton->setEnabled(false);
    buttonBox->addButton(this->editButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(this->relationEditWidget,&RelationEditWidget::changed,this,&RelationEditDialog::onRelationEdidWidgetChanged);
}

int RelationEditDialog::exec()
{
    int retVal = QDialog::exec();

    if (retVal == QDialog::Accepted)
    {
        FRelation relation = this->relationEditWidget->getRelation();
        RToolInput toolInput;
        toolInput.addAction(FToolAction::modifyRelation(this->familyTree,relation));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }

    return retVal;
}

void RelationEditDialog::onRelationEdidWidgetChanged(const FRelation &)
{
    this->editButton->setEnabled(true);
    this->editButton->setDefault(true);
}
