#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "diagram_dialog.h"

DiagramDialog::DiagramDialog(FTree *familyTree, const QUuid &itemId, QWidget *parent)
    : QDialog{parent}
    , familyTree{familyTree}
    , itemId{itemId}
{
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
}

void DiagramDialog::setItemId(const QUuid &itemId)
{
    if (this->itemId == itemId)
    {
        return;
    }
    this->itemId = itemId;
    this->setWindowTitle(this->itemId);
    this->diagram->setItemId(this->itemId);

    emit this->itemIdChanged(this->itemId);
}

void DiagramDialog::setWindowTitle(const QUuid &itemId)
{
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
}

void DiagramDialog::onItemIdChanged(const QUuid &itemId)
{
    this->setItemId(itemId);
}

void DiagramDialog::opPersonRemoved(const QUuid personId)
{
    if (personId == this->itemId)
    {
        this->done(QDialog::Rejected);
    }
}

void DiagramDialog::opRelationRemoved(const QUuid relationId)
{
    if (relationId == this->itemId)
    {
        this->done(QDialog::Rejected);
    }
}

void DiagramDialog::onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds)
{
    emit this->personSelectionChanged(selectedPersonsIds);
}

void DiagramDialog::onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds)
{
    emit this->relationSelectionChanged(selectedRelationsIds);
}
