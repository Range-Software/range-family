#include <QCheckBox>
#include <QHBoxLayout>
#include <QShortcut>
#include <QVBoxLayout>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include <rgl_message_box.h>
#include <rgl_icon.h>

#include <rgl_action_list.h>

#include "action.h"
#include "application.h"
#include "diagram_dialog.h"
#include "relation_add_dialog.h"
#include "relation_edit_dialog.h"
#include "date_tree_widget_item.h"
#include "relations_list_widget.h"

RelationsListWidget::RelationsListWidget(FTree *familyTree, QWidget *parent)
    : QWidget(parent)
    , familyTree(familyTree)
{
    RActionList *actionList = Application::instance()->getActionList();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    // Title is shown by the dock holding the list.
    this->header = new PaneHeaderWidget(QString());
    this->header->addButton(actionList->getAction(Action::ACTION_EDIT_RELATION_ADD));
    this->header->addButton(actionList->getAction(Action::ACTION_EDIT_RELATION_EDIT));
    this->header->addButton(actionList->getAction(Action::ACTION_EDIT_RELATION_REMOVE));
    mainLayout->addWidget(this->header);

    QList<RTreeWidget::ColumnInfoItem> columnInfo;
    for (int type=ColumnId;type<NumberOfColumns;type++)
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

    this->scrollBarWithMarkers = new ScrollBarWithMarkers(Qt::Vertical, this->treeWidget);
    this->treeWidget->setVerticalScrollBar(this->scrollBarWithMarkers);

    RApplicationSettings *applicationSettings = Application::instance()->getApplicationSettings();

    QHBoxLayout *optionsLayout = new QHBoxLayout;
    mainLayout->addLayout(optionsLayout);

    this->keepHighlightedCheckBox = new QCheckBox(tr("Always show highlighted"));
    this->keepHighlightedCheckBox->setToolTip(tr("Relations of selected persons stay visible even if they do not match the filter."));
    this->keepHighlightedCheckBox->setChecked(applicationSettings->value("editor/relationsKeepHighlighted",true).toBool());
    optionsLayout->addWidget(this->keepHighlightedCheckBox);
    this->header->setKeepHighlighted(this->keepHighlightedCheckBox->isChecked());

    this->onlyHighlightedCheckBox = new QCheckBox(tr("Only highlighted"));
    this->onlyHighlightedCheckBox->setToolTip(tr("Show only relations of selected persons. All relations are shown while no person is selected."));
    this->onlyHighlightedCheckBox->setChecked(applicationSettings->value("editor/relationsOnlyHighlighted",false).toBool());
    optionsLayout->addWidget(this->onlyHighlightedCheckBox);
    this->header->setOnlyHighlighted(this->onlyHighlightedCheckBox->isChecked());

    optionsLayout->addStretch(1);

    this->populate();

    QShortcut *filterShortcut = new QShortcut(QKeySequence::Find,this);
    filterShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(filterShortcut,&QShortcut::activated,this->header,&PaneHeaderWidget::focusFilter);

    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->addAction(actionList->getAction(Action::ACTION_EDIT_RELATION_ADD));
    this->addAction(actionList->getAction(Action::ACTION_EDIT_RELATION_EDIT));
    this->addAction(actionList->getAction(Action::ACTION_EDIT_RELATION_REMOVE));

    QAction *separatorAction = new QAction(this);
    separatorAction->setSeparator(true);
    this->addAction(separatorAction);

    this->addAction(actionList->getAction(Action::ACTION_EDIT_RELATION_SHOW_TREE));

    this->onItemSelectionChanged();

    QObject::connect(this->treeWidget,&QTreeWidget::itemSelectionChanged,this,&RelationsListWidget::onItemSelectionChanged);
    QObject::connect(this->treeWidget,&QTreeWidget::itemDoubleClicked,this,&RelationsListWidget::onItemDoubleClicked);

    QObject::connect(this->header,&PaneHeaderWidget::filterChanged,this,&RelationsListWidget::applyFilter);
    QObject::connect(this->keepHighlightedCheckBox,&QCheckBox::toggled,this,&RelationsListWidget::onKeepHighlightedToggled);
    QObject::connect(this->onlyHighlightedCheckBox,&QCheckBox::toggled,this,&RelationsListWidget::onOnlyHighlightedToggled);

    // Highlighted items are colored for the color scheme.
    QObject::connect(RApplication::instance(),&RApplication::styleApplied,this,&RelationsListWidget::onStyleApplied);

    QObject::connect(this->familyTree,&FTree::relationAdded,this,&RelationsListWidget::addRelation);
    QObject::connect(this->familyTree,&FTree::relationChanged,this,&RelationsListWidget::changeRelation);
    QObject::connect(this->familyTree,&FTree::relationRemoved,this,&RelationsListWidget::removeRelation);

    QObject::connect(this->familyTree,&FTree::personChanged,this,&RelationsListWidget::changePerson);

    QObject::connect(this->familyTree,&FTree::fileLoaded,this,&RelationsListWidget::onFileLoaded);
}

void RelationsListWidget::highlightRelations(const QList<QUuid> &ids)
{
    const QSet<QUuid> idsSet(ids.constBegin(),ids.constEnd());

    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        PaneHeaderWidget::setItemHighlighted(item,idsSet.contains(item->data(ColumnId,Qt::UserRole).toUuid()));
    }

    // Highlighted items may be shown regardless of the filter, or be the only ones shown.
    this->applyFilter();
}

void RelationsListWidget::selectRelation(const QUuid &id)
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        if (item->data(ColumnId,Qt::UserRole).toUuid() != id)
        {
            continue;
        }
        if (item->isHidden())
        {
            this->header->clearFilter();
        }
        if (item->isHidden())
        {
            // Hidden by showing only highlighted relations.
            return;
        }
        if (!item->isSelected() || this->treeWidget->selectedItems().count() != 1)
        {
            this->treeWidget->clearSelection();
            item->setSelected(true);
        }
        this->treeWidget->setCurrentItem(item);
        this->treeWidget->scrollToItem(item);
        return;
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
        QTreeWidgetItem *item = new DateTreeWidgetItem(this->treeWidget);
        this->setTreeWidgetItemValues(item,relation);
    }
    this->applyFilter();

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
    DateTreeWidgetItem::setDate(item,ColumnBeginDate,relation.getBegin().getDate());
    item->setText(ColumnBeginAddress,relation.getBegin().getPlace().getAddress().toString());
    item->setText(ColumnBeginCoordinates,relation.getBegin().getPlace().getGeoCoordinates().toString());
    DateTreeWidgetItem::setDate(item,ColumnEndDate,relation.getEnd().getDate());
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

    this->updateActionsAvailability();

    emit this->selectionChanged(ids);
}

void RelationsListWidget::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
    QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());
    FRelation relation(this->familyTree->findRelation(id));

    RelationEditDialog relationEditDialog(this->familyTree,relation,this);
    relationEditDialog.exec();
}

void RelationsListWidget::addNewRelation()
{
    RelationAddDialog relationAddDialog(this->familyTree,FRelation(),this);
    relationAddDialog.exec();
}

void RelationsListWidget::editSelectedRelations()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    bool edit = true;

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to edit all selected relations?");
        int response = RMessageBox::question(this,tr("Edit relations"),question);
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

void RelationsListWidget::removeSelectedRelations()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to remove all selected relations?") + " (" + QLocale().toString(selectedItems.count()) + ")";
    }
    else
    {
        question = tr("Are you sure you want to remove the selected relation?");
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
    QTreeWidgetItem *item = new DateTreeWidgetItem(this->treeWidget);
    this->setTreeWidgetItemValues(item,relation);
    this->applyFilter();
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
            this->applyFilter();
            return;
        }
    }
}

void RelationsListWidget::applyFilter()
{
    this->header->filterItems(this->treeWidget);
    this->updateMarkers();
}

void RelationsListWidget::updateMarkers()
{
    // Scroll bar spans only items which are visible.
    QList<QTreeWidgetItem*> visibleItems;
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        if (!item->isHidden())
        {
            visibleItems.append(item);
        }
    }

    QList<double> markerPositions;
    for (int i=0;i<visibleItems.count();i++)
    {
        if (PaneHeaderWidget::isItemHighlighted(visibleItems.at(i)))
        {
            markerPositions.append(static_cast<double>(i) / visibleItems.count());
        }
    }

    this->scrollBarWithMarkers->setMarkers(markerPositions);
}

void RelationsListWidget::onKeepHighlightedToggled(bool checked)
{
    Application::instance()->getApplicationSettings()->setValue("editor/relationsKeepHighlighted",checked);
    this->header->setKeepHighlighted(checked);
    this->applyFilter();
}

void RelationsListWidget::onOnlyHighlightedToggled(bool checked)
{
    Application::instance()->getApplicationSettings()->setValue("editor/relationsOnlyHighlighted",checked);
    this->header->setOnlyHighlighted(checked);
    this->applyFilter();
}

void RelationsListWidget::onStyleApplied()
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        PaneHeaderWidget::setItemHighlighted(item,PaneHeaderWidget::isItemHighlighted(item));
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
            this->applyFilter();
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

void RelationsListWidget::showSelectedRelationsDiagram()
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

void RelationsListWidget::updateActionsAvailability()
{
    int nSelected = this->treeWidget->selectedItems().count();

    RActionList *actionList = Application::instance()->getActionList();
    actionList->getAction(Action::ACTION_EDIT_RELATION_EDIT)->setEnabled(nSelected > 0);
    actionList->getAction(Action::ACTION_EDIT_RELATION_REMOVE)->setEnabled(nSelected > 0);
    actionList->getAction(Action::ACTION_EDIT_RELATION_SHOW_TREE)->setEnabled(nSelected > 0);
}
