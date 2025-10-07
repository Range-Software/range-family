#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include <rbl_logger.h>

#include "diagram_dialog.h"

DiagramDialog::DiagramDialog(FTree *familyTree, const QUuid &itemId, QWidget *parent)
    : QDialog{parent}
    , familyTree{familyTree}
    , itemId{itemId}
{
    R_LOG_TRACE_IN;
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(this->itemId);
    this->setModal(false);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout (mainLayout);

    this->diagram = new DiagramWidget(this->familyTree,this->itemId,this);
    mainLayout->addWidget(this->diagram);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);

    QObject::connect(this->diagram,&DiagramWidget::itemIdChanged,this,&DiagramDialog::onItemIdChanged);
    QObject::connect(this->diagram,&DiagramWidget::personSelectionChanged,this,&DiagramDialog::onPersonSelectionChanged);
    QObject::connect(this->diagram,&DiagramWidget::relationSelectionChanged,this,&DiagramDialog::onRelationSelectionChanged);

    QObject::connect(this->familyTree,&FTree::personRemoved,this,&DiagramDialog::opPersonRemoved);
    QObject::connect(this->familyTree,&FTree::relationRemoved,this,&DiagramDialog::opRelationRemoved);

    R_LOG_TRACE_OUT;
}

void DiagramDialog::setItemId(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    if (this->itemId == itemId)
    {
        R_LOG_TRACE_OUT;
        return;
    }
    this->itemId = itemId;
    this->setWindowTitle(this->itemId);
    this->diagram->setItemId(this->itemId);

    emit this->itemIdChanged(this->itemId);
    R_LOG_TRACE_OUT;
}

void DiagramDialog::setWindowTitle(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    QString itemName;
    if (this->familyTree->containsPerson(itemId))
    {
        itemName = this->familyTree->findPerson(itemId).getName().toString();
    }
    else if (this->familyTree->containsRelation(itemId))
    {
        itemName = tr("Relation");
    }
    itemName += " " + itemId.toString(QUuid::WithBraces);

    this->QDialog::setWindowTitle(tr("Family tree") + " - " + itemName);
    R_LOG_TRACE_OUT;
}

void DiagramDialog::onItemIdChanged(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    this->setItemId(itemId);
    R_LOG_TRACE_OUT;
}

void DiagramDialog::opPersonRemoved(const QUuid personId)
{
    R_LOG_TRACE_IN;
    if (personId == this->itemId)
    {
        this->done(QDialog::Rejected);
    }
    R_LOG_TRACE_OUT;
}

void DiagramDialog::opRelationRemoved(const QUuid relationId)
{
    R_LOG_TRACE_IN;
    if (relationId == this->itemId)
    {
        this->done(QDialog::Rejected);
    }
    R_LOG_TRACE_OUT;
}

void DiagramDialog::onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds)
{
    R_LOG_TRACE_IN;
    emit this->personSelectionChanged(selectedPersonsIds);

    R_LOG_TRACE_OUT;
}

void DiagramDialog::onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds)
{
    R_LOG_TRACE_IN;
    emit this->relationSelectionChanged(selectedRelationsIds);
    R_LOG_TRACE_OUT;
}
