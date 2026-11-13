#include <QCheckBox>
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
#include "person_add_dialog.h"
#include "person_edit_dialog.h"
#include "date_tree_widget_item.h"
#include "persons_list_widget.h"
#include "person_suggested_dates_dialog.h"

#define _FP_TR(qstr) qApp->translate("FPerson",qstr.toUtf8().constData())

PersonsListWidget::PersonsListWidget(FTree *familyTree, QWidget *parent)
    : QWidget(parent)
    , familyTree(familyTree)
{
    RActionList *actionList = Application::instance()->getActionList();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    // Explicit scope, lupdate does not find the context of plain tr() in this constructor.
    this->header = new PaneHeaderWidget(PersonsListWidget::tr("Persons"));
    this->header->addButton(actionList->getAction(Action::ACTION_EDIT_PERSON_ADD));
    this->header->addButton(actionList->getAction(Action::ACTION_EDIT_PERSON_EDIT));
    this->header->addButton(actionList->getAction(Action::ACTION_EDIT_PERSON_REMOVE));
    mainLayout->addWidget(this->header);

    QList<RTreeWidget::ColumnInfoItem> columnInfo;
    for (int type=ColumnId;type<NumberOfColumns;type++)
    {
        RTreeWidget::ColumnInfoItem columnInfoItem;
        columnInfoItem.column = type;
        columnInfoItem.title = PersonsListWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = (type == ColumnId || type == ColumnBirthCoordinates || type == ColumnDeathCoordinates);
        columnInfoItem.available = true;
        columnInfo.append(columnInfoItem);
    }

    this->treeWidget = new RTreeWidget(columnInfo);
    this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->treeWidget->setAlternatingRowColors(true);
    mainLayout->addWidget(this->treeWidget);

    this->scrollBarWithMarkers = new ScrollBarWithMarkers(Qt::Vertical, this->treeWidget);
    this->treeWidget->setVerticalScrollBar(this->scrollBarWithMarkers);

    this->keepHighlightedCheckBox = new QCheckBox(PersonsListWidget::tr("Always show highlighted"));
    this->keepHighlightedCheckBox->setToolTip(PersonsListWidget::tr("Persons of selected relations stay visible even if they do not match the filter."));
    this->keepHighlightedCheckBox->setChecked(Application::instance()->getApplicationSettings()->value("editor/personsKeepHighlighted",true).toBool());
    mainLayout->addWidget(this->keepHighlightedCheckBox);
    this->header->setKeepHighlighted(this->keepHighlightedCheckBox->isChecked());

    this->populate();

    QShortcut *filterShortcut = new QShortcut(QKeySequence::Find,this);
    filterShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(filterShortcut,&QShortcut::activated,this->header,&PaneHeaderWidget::focusFilter);

    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_ADD));
    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_EDIT));
    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_REMOVE));

    QAction *separator0Action = new QAction(this);
    separator0Action->setSeparator(true);
    this->addAction(separator0Action);

    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_SHOW_TREE));

    QAction *separator1Action = new QAction(this);
    separator1Action->setSeparator(true);
    this->addAction(separator1Action);

    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_SET_ACTIVE));

    QAction *separator2Action = new QAction(this);
    separator2Action->setSeparator(true);
    this->addAction(separator2Action);

    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_MERGE));
    this->addAction(actionList->getAction(Action::ACTION_EDIT_PERSON_SUGGEST_DATES));

    this->onItemSelectionChanged();

    QObject::connect(this->treeWidget,&QTreeWidget::itemSelectionChanged,this,&PersonsListWidget::onItemSelectionChanged);
    QObject::connect(this->treeWidget,&QTreeWidget::itemDoubleClicked,this,&PersonsListWidget::onItemDoubleClicked);

    QObject::connect(this->header,&PaneHeaderWidget::filterChanged,this,&PersonsListWidget::applyFilter);
    QObject::connect(this->keepHighlightedCheckBox,&QCheckBox::toggled,this,&PersonsListWidget::onKeepHighlightedToggled);

    // Highlighted items are colored for the color scheme.
    QObject::connect(RApplication::instance(),&RApplication::styleApplied,this,&PersonsListWidget::onStyleApplied);

    QObject::connect(this->familyTree,&FTree::personAdded,this,&PersonsListWidget::addPerson);
    QObject::connect(this->familyTree,&FTree::personChanged,this,&PersonsListWidget::changePerson);
    QObject::connect(this->familyTree,&FTree::personRemoved,this,&PersonsListWidget::removePerson);

    QObject::connect(this->familyTree,&FTree::fileLoaded,this,&PersonsListWidget::onFileLoaded);
}

void PersonsListWidget::highlightPersons(const QList<QUuid> &ids)
{
    const QSet<QUuid> idsSet(ids.constBegin(),ids.constEnd());

    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        PaneHeaderWidget::setItemHighlighted(item,idsSet.contains(item->data(ColumnId,Qt::UserRole).toUuid()));
    }

    // Highlighted items may be shown regardless of the filter.
    this->applyFilter();
}

void PersonsListWidget::selectPerson(const QUuid &id)
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

QList<QUuid> PersonsListWidget::getSelectedIds() const
{
    QList<QUuid> ids;

    foreach (QTreeWidgetItem *item, this->treeWidget->selectedItems())
    {
        ids.append(item->data(ColumnId,Qt::UserRole).toUuid());
    }

    return ids;
}

void PersonsListWidget::populate()
{
    this->treeWidget->clear();
    foreach (const QUuid &personId, this->familyTree->getPersons())
    {
        FPerson person = this->familyTree->findPerson(personId);
        QTreeWidgetItem *item = new DateTreeWidgetItem(this->treeWidget);
        PersonsListWidget::setTreeWidgetItemValues(item,person);
    }
    this->applyFilter();

    treeWidget->resizeColumnToContents(ColumnFirstName);
    treeWidget->resizeColumnToContents(ColumnLastName);

    this->onActivePersonIdChanged(Application::instance()->getSession()->getActivePersonId());
}

QString PersonsListWidget::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnId:               return tr("ID");
        case ColumnSex:              return tr("Sex");
        case ColumnFirstName:        return tr("First name");
        case ColumnMiddleName:       return tr("Middle name");
        case ColumnLastName:         return tr("Last name");
        case ColumnMaidenName:       return tr("Maiden name");
        case ColumnBirthDate:        return tr("Birth date");
        case ColumnBirthAddress:     return tr("Birth address");
        case ColumnBirthCoordinates: return tr("Birth coordinates");
        case ColumnDeathDate:        return tr("Death date");
        case ColumnDeathAddress:     return tr("Death address");
        case ColumnDeathCoordinates: return tr("Death coordinates");
        default:                     return QString();
    }
}

void PersonsListWidget::setTreeWidgetItemValues(QTreeWidgetItem *item, const FPerson &person)
{
    item->setText(ColumnId,person.getId().toString(QUuid::StringFormat::WithoutBraces));
    item->setData(ColumnId,Qt::UserRole,QVariant(person.getId()));
    item->setText(ColumnSex,_FP_TR(FPerson::sexKeyToDesc(person.getSex())));
    item->setText(ColumnFirstName,person.getName().getFirstName());
    item->setText(ColumnMiddleName,person.getName().getMiddleName());
    item->setText(ColumnLastName,person.getName().getLastName());
    item->setText(ColumnMaidenName,person.getName().getMaidenName());
    DateTreeWidgetItem::setDate(item,ColumnBirthDate,person.getBirth().getDate());
    item->setText(ColumnBirthAddress,person.getBirth().getPlace().getAddress().toString());
    item->setText(ColumnBirthCoordinates,person.getBirth().getPlace().getGeoCoordinates().toString());
    DateTreeWidgetItem::setDate(item,ColumnDeathDate,person.getDeath().getDate());
    item->setText(ColumnDeathAddress,person.getDeath().getPlace().getAddress().toString());
    item->setText(ColumnDeathCoordinates,person.getDeath().getPlace().getGeoCoordinates().toString());
}

void PersonsListWidget::showColumn(int column, bool checked)
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

void PersonsListWidget::onItemSelectionChanged()
{
    QList<QUuid> ids = this->getSelectedIds();

    this->updateActionsAvailability();

    emit this->selectionChanged(ids);
}

void PersonsListWidget::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
    QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());
    FPerson person(this->familyTree->findPerson(id));

    PersonEditDialog personEditDialog(this->familyTree,person,this);
    personEditDialog.exec();
}

void PersonsListWidget::addNewPerson()
{
    PersonAddDialog personAddDialog(this->familyTree,FPerson(),this);
    personAddDialog.exec();
}

void PersonsListWidget::editSelectedPersons()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    bool edit = true;

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to edit all selected persons?");
        int response = RMessageBox::question(this,tr("Edit persons"),question);
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
            FPerson person(this->familyTree->findPerson(id));

            PersonEditDialog personEditDialog(this->familyTree,person,this);
            personEditDialog.exec();
        }
    }
}

void PersonsListWidget::removeSelectedPersons()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to remove all selected persons?") + " (" + QLocale().toString(selectedItems.count()) + ")";
    }
    else
    {
        question = tr("Are you sure you want to remove the selected person?");
    }

    int response = RMessageBox::question(this,tr("Remove person"),question);
    if (response == RMessageBox::Yes)
    {
        foreach (QTreeWidgetItem *item, selectedItems)
        {
            QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());

            RToolInput toolInput;
            toolInput.addAction(FToolAction::removePerson(this->familyTree,id));
            RToolTask *toolTask = new RToolTask(toolInput);
            RJobManager::getInstance().submit(toolTask);
        }
    }
}

void PersonsListWidget::addPerson(const FPerson &person)
{
    QTreeWidgetItem *item = new DateTreeWidgetItem(this->treeWidget);
    PersonsListWidget::setTreeWidgetItemValues(item,person);
    this->applyFilter();
}

void PersonsListWidget::changePerson(const FPerson &person)
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        QUuid id = item->data(ColumnId,Qt::UserRole).toUuid();
        if (person.getId() == id)
        {
            PersonsListWidget::setTreeWidgetItemValues(this->treeWidget->topLevelItem(i),person);
            this->applyFilter();
            return;
        }
    }
}

void PersonsListWidget::applyFilter()
{
    this->header->filterItems(this->treeWidget);
    this->updateMarkers();
}

void PersonsListWidget::updateMarkers()
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
    QList<double> activeMarkerPositions;
    for (int i=0;i<visibleItems.count();i++)
    {
        const double position = static_cast<double>(i) / visibleItems.count();
        if (PaneHeaderWidget::isItemHighlighted(visibleItems.at(i)))
        {
            markerPositions.append(position);
        }
        if (visibleItems.at(i)->data(ColumnId,Qt::UserRole).toUuid() == this->activePersonId)
        {
            activeMarkerPositions.append(position);
        }
    }

    this->scrollBarWithMarkers->setMarkers(markerPositions);
    this->scrollBarWithMarkers->setActiveMarkers(activeMarkerPositions);
}

void PersonsListWidget::onKeepHighlightedToggled(bool checked)
{
    Application::instance()->getApplicationSettings()->setValue("editor/personsKeepHighlighted",checked);
    this->header->setKeepHighlighted(checked);
    this->applyFilter();
}

void PersonsListWidget::onStyleApplied()
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        PaneHeaderWidget::setItemHighlighted(item,PaneHeaderWidget::isItemHighlighted(item));
    }
}

void PersonsListWidget::removePerson(const QUuid &personId)
{
    int nItems = this->treeWidget->topLevelItemCount();
    for (int i=0;i<nItems;i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        QUuid id = item->data(ColumnId,Qt::UserRole).toUuid();
        if (personId == id)
        {
            delete this->treeWidget->takeTopLevelItem(i);
            this->applyFilter();
            return;
        }
    }
}

void PersonsListWidget::onFileLoaded(const QString &)
{
    this->populate();
}

void PersonsListWidget::showSelectedPersonsDiagram()
{
    foreach (auto selectedId, this->getSelectedIds())
    {
        DiagramDialog *diagramDialog = new DiagramDialog(this->familyTree,selectedId,this);
        diagramDialog->show();
        QObject::connect(diagramDialog,&DiagramDialog::personSelectionChanged,this,&PersonsListWidget::personDiagramSelectionChanged);
    }
}

void PersonsListWidget::setSelectedPersonActive()
{
    QList<QUuid> selectedIds = this->getSelectedIds();

    if (selectedIds.size() > 0)
    {
        Application::instance()->getSession()->setActivePersonId(selectedIds.at(0));
    }
}

void PersonsListWidget::mergeSelectedPersons()
{
    QList<QUuid> selectedIds = this->getSelectedIds();

    if (selectedIds.size() < 2)
    {
        return;
    }

    QString question = tr("Are you sure you want to merge selected persons?");

    question += "<ul>";
    foreach (QUuid id, selectedIds)
    {
        question += "<li>" + this->familyTree->findPerson(id).getName().toString() + "</li>";
    }
    question += "</ul>";

    if (RMessageBox::question(this,tr("Merge persons"),question) == RMessageBox::Yes)
    {
        this->familyTree->mergePersons(selectedIds);
    }
}

void PersonsListWidget::suggestSelectedPersonsDates()
{
    foreach (auto selectedId, this->getSelectedIds())
    {
        FPerson person(this->familyTree->findPerson(selectedId));
        PersonSuggestedDatesDialog personSuggestedDatesDialog(this->familyTree,person,this);
        personSuggestedDatesDialog.exec();
    }
}

void PersonsListWidget::personDiagramSelectionChanged(const QList<QUuid> &)
{

}

void PersonsListWidget::onActivePersonIdChanged(const QUuid &id)
{
    this->activePersonId = id;

    int nItems = this->treeWidget->topLevelItemCount();
    for (int i = 0; i < nItems; i++)
    {
        QTreeWidgetItem *item = this->treeWidget->topLevelItem(i);
        bool isActive = (item->data(ColumnId, Qt::UserRole).toUuid() == id);
        QFont font = item->font(ColumnFirstName);
        font.setBold(isActive);
        for (int col = 0; col < NumberOfColumns; col++)
        {
            item->setFont(col, font);
        }
    }

    this->updateMarkers();
}

void PersonsListWidget::updateActionsAvailability()
{
    int nSelected = this->treeWidget->selectedItems().count();

    RActionList *actionList = Application::instance()->getActionList();
    actionList->getAction(Action::ACTION_EDIT_PERSON_EDIT)->setEnabled(nSelected > 0);
    actionList->getAction(Action::ACTION_EDIT_PERSON_REMOVE)->setEnabled(nSelected > 0);
    actionList->getAction(Action::ACTION_EDIT_PERSON_SHOW_TREE)->setEnabled(nSelected > 0);
    actionList->getAction(Action::ACTION_EDIT_PERSON_SET_ACTIVE)->setEnabled(nSelected == 1);
    actionList->getAction(Action::ACTION_EDIT_PERSON_MERGE)->setEnabled(nSelected > 1);
    actionList->getAction(Action::ACTION_EDIT_PERSON_SUGGEST_DATES)->setEnabled(nSelected > 0);
}
