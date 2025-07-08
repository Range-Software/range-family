#include <QHBoxLayout>
#include <QVBoxLayout>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include <rgl_message_box.h>

#include "diagram_dialog.h"
#include "relation_add_dialog.h"
#include "relation_edit_dialog.h"
#include "relations_list_widget.h"

RelationsListWidget::RelationsListWidget(FTree *familyTree, QWidget *parent)
    : QWidget(parent)
    , familyTree(familyTree)
{
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon editIcon(":/icons/action/pixmaps/range-edit.svg");
    QIcon removeIcon(":/icons/action/pixmaps/range-remove.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QList<RTreeWidget::ColumnInfoItem> columnInfo;
    for (int type=ColumnId;type<NunberOfColumns;type++)
    {
        RTreeWidget::ColumnInfoItem columnInfoItem;
        columnInfoItem.column = type;
        columnInfoItem.title = RelationsListWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = (type == ColumnId || type == ColumnBeginCoordinates || type == ColumnEndCoordinates || type == ColumnEndAddress);
        columnInfoItem.available = true;
        columnInfo.append(columnInfoItem);
    }

    this->treeWidget = new RTreeWidget(columnInfo);
    this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->treeWidget->setAlternatingRowColors(true);
    mainLayout->addWidget(this->treeWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);

    this->relationAddButton = new QPushButton(addIcon,tr("Add"));
    buttonLayout->addWidget(this->relationAddButton);

    this->relationEditButton = new QPushButton(editIcon,tr("Edit"));
    buttonLayout->addWidget(this->relationEditButton);

    this->relationRemoveButton = new QPushButton(removeIcon,tr("Remove"));
    buttonLayout->addWidget(this->relationRemoveButton);

    this->populate();

    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->showTreeAction = this->addAction(RelationsListWidget::tr("Show family tree"));
    QObject::connect(this->showTreeAction,&QAction::triggered,this,&RelationsListWidget::onShowDiagramDialog);

    this->onItemSelectionChanged();

    QObject::connect(this->treeWidget,&QTreeWidget::itemSelectionChanged,this,&RelationsListWidget::onItemSelectionChanged);
    QObject::connect(this->treeWidget,&QTreeWidget::itemDoubleClicked,this,&RelationsListWidget::onItemDoubleClicked);

    QObject::connect(this->relationAddButton,&QPushButton::clicked,this,&RelationsListWidget::onRelationAddButtonClicked);
    QObject::connect(this->relationEditButton,&QPushButton::clicked,this,&RelationsListWidget::onRelationEditButtonClicked);
    QObject::connect(this->relationRemoveButton,&QPushButton::clicked,this,&RelationsListWidget::onRelationRemoveButtonClicked);

    QObject::connect(this->familyTree,&FTree::relationAdded,this,&RelationsListWidget::addRelation);
    QObject::connect(this->familyTree,&FTree::relationChanged,this,&RelationsListWidget::changeRelation);
    QObject::connect(this->familyTree,&FTree::relationRemoved,this,&RelationsListWidget::removeRelation);

    QObject::connect(this->familyTree,&FTree::personChanged,this,&RelationsListWidget::changePerson);

    QObject::connect(this->familyTree,&FTree::fileLoaded,this,&RelationsListWidget::onFileLoaded);
}

void RelationsListWidget::highlightRelations(const QList<QUuid> &ids)
{
    QTreeWidgetItemIterator it(this->treeWidget);
    while (*it)
    {
        if (ids.contains(QUuid((*it)->text(ColumnId))))
        {
            for (int column=0;column<NunberOfColumns;column++)
            {
                (*it)->setBackground(column,QBrush(Qt::yellow));
                (*it)->setForeground(column,QBrush(Qt::black));
            }
        }
        else
        {
            for (int column=0;column<NunberOfColumns;column++)
            {
                (*it)->setBackground(column,QBrush());
                (*it)->setForeground(column,QBrush());
            }
        }
        ++it;
    }
}

QList<QUuid> RelationsListWidget::getSelectedIds() const
{
    QList<QUuid> ids;

    foreach (QTreeWidgetItem *item, this->treeWidget->selectedItems())
    {
        ids.append(item->data(ColumnId,Qt::UserRole).toUuid());
    }

    return ids;
}

void RelationsListWidget::populate()
{
    this->treeWidget->clear();
    foreach (const QUuid &relationId, this->familyTree->getRelations())
    {
        FRelation relation = this->familyTree->findRelation(relationId);
        this->addRelation(relation);
    }

    treeWidget->resizeColumnToContents(ColumnPartners);
    treeWidget->resizeColumnToContents(ColumnChildren);
}

QString RelationsListWidget::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnId:               return tr("ID");
        case ColumnPartners:         return tr("Partners");
        case ColumnChildren:         return tr("Children");
        case ColumnBeginDate:        return tr("Begin date");
        case ColumnBeginAddress:     return tr("Begin address");
        case ColumnBeginCoordinates: return tr("Begin coordinates");
        case ColumnEndDate:          return tr("End date");
        case ColumnEndAddress:       return tr("End address");
        case ColumnEndCoordinates:   return tr("End coordinates");
        default:                     return QString();
    }
}

void RelationsListWidget::setTreeWidgetItemValues(QTreeWidgetItem *item, const FRelation &relation) const
{
    item->setText(ColumnId,relation.getId().toString(QUuid::StringFormat::WithoutBraces));
    item->setData(ColumnId,Qt::UserRole,QVariant(relation.getId()));
    item->setText(ColumnPartners,this->getPartnersListString(relation));
    item->setText(ColumnChildren,this->getChildrenListString(relation));
    item->setData(ColumnBeginDate,Qt::DisplayRole,relation.getBegin().getDate().toDate());
    item->setText(ColumnBeginAddress,relation.getBegin().getPlace().getAddress().toString());
    item->setText(ColumnBeginCoordinates,relation.getBegin().getPlace().getGeoCoordinates().toString());
    item->setData(ColumnEndDate,Qt::DisplayRole,relation.getEnd().getDate().toDate());
    item->setText(ColumnEndAddress,relation.getEnd().getPlace().getAddress().toString());
    item->setText(ColumnEndCoordinates,relation.getEnd().getPlace().getGeoCoordinates().toString());
}

QString RelationsListWidget::getPartnersListString(const FRelation &relation) const
{
    const QList<QUuid> partnersId = relation.getPartners();
    QString partnersString;
    foreach (const QUuid &partnerId, partnersId)
    {
        FPerson person = this->familyTree->findPerson(partnerId);
        if (!partnersString.isEmpty())
        {
            partnersString += "\n";
        }
        partnersString += person.getName().toString();
    }
    return partnersString;
}

QString RelationsListWidget::getChildrenListString(const FRelation &relation) const
{
    const QList<QUuid> childrenId = relation.getChildren();
    QString childrenString;
    foreach (const QUuid &childId, childrenId)
    {
        FPerson person = this->familyTree->findPerson(childId);
        if (!childrenString.isEmpty())
        {
            childrenString += "\n";
        }
        childrenString +=  person.getName().toString();
    }
    return childrenString;
}

void RelationsListWidget::showColumn(int column, bool checked)
{
    if (checked)
    {
        this->treeWidget->showColumn(column);
    }
    else
    {
        this->treeWidget->hideColumn(column);
    }
}

void RelationsListWidget::onItemSelectionChanged()
{
    QList<QUuid> ids = this->getSelectedIds();

    this->relationEditButton->setEnabled(ids.count() > 0);
    this->relationRemoveButton->setEnabled(ids.count() > 0);

    emit this->selectionChanged(ids);
}

void RelationsListWidget::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
    QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());
    FRelation relation(this->familyTree->findRelation(id));

    RelationEditDialog relationEditDialog(this->familyTree,relation,this);
    relationEditDialog.exec();
}

void RelationsListWidget::onRelationAddButtonClicked()
{
    RelationAddDialog relationAddDialog(this->familyTree,FRelation(),this);
    relationAddDialog.exec();
}

void RelationsListWidget::onRelationEditButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    bool edit = true;

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to edit all selected relations?");
        int response = RMessageBox::question(this,tr("Remove relation"),question);
        if (response != RMessageBox::Yes)
        {
            edit = false;
        }
    }

    if (edit)
    {
        foreach (QTreeWidgetItem *item, selectedItems)
        {
            QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());
            FRelation relation(this->familyTree->findRelation(id));

            RelationEditDialog relationEditDialog(this->familyTree,relation,this);
            relationEditDialog.exec();
        }
    }
}

void RelationsListWidget::onRelationRemoveButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to remove all selected relations?") + " (" + QLocale().toString(selectedItems.count()) + ")";
    }
    else
    {
        question = tr("Are you sure you want to remove selected relation?");
    }

    int response = RMessageBox::question(this,tr("Remove relation"),question);
    if (response == RMessageBox::Yes)
    {
        foreach (QTreeWidgetItem *item, selectedItems)
        {
            QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());

            RToolInput toolInput;
            toolInput.addAction(FToolAction::removeRelation(this->familyTree,id));
            RToolTask *toolTask = new RToolTask(toolInput);
            RJobManager::getInstance().submit(toolTask);
        }
    }
}

void RelationsListWidget::addRelation(const FRelation &relation)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(this->treeWidget);
    this->setTreeWidgetItemValues(item,relation);
}

void RelationsListWidget::changeRelation(const FRelation &relation)
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        QUuid id = item->data(ColumnId,Qt::UserRole).toUuid();
        if (relation.getId() == id)
        {
            RelationsListWidget::setTreeWidgetItemValues(this->treeWidget->topLevelItem(i),relation);
            return;
        }
    }
}

void RelationsListWidget::removeRelation(const QUuid &relationId)
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        QUuid id = item->data(ColumnId,Qt::UserRole).toUuid();
        if (relationId == id)
        {
            delete this->treeWidget->takeTopLevelItem(i);
            return;
        }
    }
}

void RelationsListWidget::changePerson(const FPerson &person)
{
    QList<QUuid> relationIds = this->familyTree->findPersonRelations(person.getId());
    foreach (const QUuid &relationId, relationIds)
    {
        this->changeRelation(this->familyTree->findRelation(relationId));
    }
}

void RelationsListWidget::onFileLoaded(const QString &)
{
    this->populate();
}

void RelationsListWidget::onShowDiagramDialog()
{
    foreach (auto selectedId, this->getSelectedIds())
    {
        DiagramDialog *diagramDialog = new DiagramDialog(this->familyTree,selectedId,this);
        diagramDialog->show();
        QObject::connect(diagramDialog,&DiagramDialog::relationSelectionChanged,this,&RelationsListWidget::relationDiagramSelectionChanged);
    }
}

void RelationsListWidget::relationDiagramSelectionChanged(const QList<QUuid> &)
{

}
