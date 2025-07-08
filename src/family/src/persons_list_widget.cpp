#include <QHBoxLayout>
#include <QVBoxLayout>

#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include <rgl_message_box.h>

#include "diagram_dialog.h"
#include "person_add_dialog.h"
#include "person_edit_dialog.h"
#include "persons_list_widget.h"
#include "person_suggeted_dates_dialog.h"

#define _FP_TR(qstr) qApp->translate("FPerson",qstr.toUtf8().constData())

PersonsListWidget::PersonsListWidget(FTree *familyTree, QWidget *parent)
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
        columnInfoItem.title = PersonsListWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = (type == ColumnId || type == ColumnBirthCoordinates || type == ColumnDeathCoordinates);
        columnInfoItem.available = true;
        columnInfo.append(columnInfoItem);
    }

    this->treeWidget = new RTreeWidget(columnInfo);
    this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->treeWidget->setAlternatingRowColors(true);
    mainLayout->addWidget(this->treeWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);

    this->personAddButton = new QPushButton(addIcon,PersonsListWidget::tr("Add"));
    buttonLayout->addWidget(this->personAddButton);

    this->personEditButton = new QPushButton(editIcon,PersonsListWidget::tr("Edit"));
    buttonLayout->addWidget(this->personEditButton);

    this->personRemoveButton = new QPushButton(removeIcon,PersonsListWidget::tr("Remove"));
    buttonLayout->addWidget(this->personRemoveButton);

    this->populate();

    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->showTreeAction = this->addAction(PersonsListWidget::tr("Show family tree"));
    QObject::connect(this->showTreeAction,&QAction::triggered,this,&PersonsListWidget::onShowDiagramDialog);

    QAction *separatorAction = new QAction;
    separatorAction->setSeparator(true);
    this->addAction(separatorAction);

    this->mergePersonsAction = this->addAction(PersonsListWidget::tr("Merge selected persons"));
    QObject::connect(this->mergePersonsAction,&QAction::triggered,this,&PersonsListWidget::onMergePersonsAction);

    this->suggestPersonBirthAndDeathDatesAction = this->addAction(PersonsListWidget::tr("Suggest birth and death dates"));
    QObject::connect(this->suggestPersonBirthAndDeathDatesAction,&QAction::triggered,this,&PersonsListWidget::onSuggestPersonBirthAndDeathDatesAction);

    this->onItemSelectionChanged();

    QObject::connect(this->treeWidget,&QTreeWidget::itemSelectionChanged,this,&PersonsListWidget::onItemSelectionChanged);
    QObject::connect(this->treeWidget,&QTreeWidget::itemDoubleClicked,this,&PersonsListWidget::onItemDoubleClicked);

    QObject::connect(this->personAddButton,&QPushButton::clicked,this,&PersonsListWidget::onPersonAddButtonClicked);
    QObject::connect(this->personEditButton,&QPushButton::clicked,this,&PersonsListWidget::onPersonEditButtonClicked);
    QObject::connect(this->personRemoveButton,&QPushButton::clicked,this,&PersonsListWidget::onPersonRemoveButtonClicked);

    QObject::connect(this->familyTree,&FTree::personAdded,this,&PersonsListWidget::addPerson);
    QObject::connect(this->familyTree,&FTree::personChanged,this,&PersonsListWidget::changePerson);
    QObject::connect(this->familyTree,&FTree::personRemoved,this,&PersonsListWidget::removePerson);

    QObject::connect(this->familyTree,&FTree::fileLoaded,this,&PersonsListWidget::onFileLoaded);
}

void PersonsListWidget::highlightPersons(const QList<QUuid> &ids)
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
        this->addPerson(person);
    }

    treeWidget->resizeColumnToContents(ColumnFirstName);
    treeWidget->resizeColumnToContents(ColumnLastName);
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
    item->setData(ColumnBirthDate,Qt::DisplayRole,person.getBirth().getDate().toDate());
    item->setText(ColumnBirthAddress,person.getBirth().getPlace().getAddress().toString());
    item->setText(ColumnBirthCoordinates,person.getBirth().getPlace().getGeoCoordinates().toString());
    item->setData(ColumnDeathDate,Qt::DisplayRole,person.getDeath().getDate().toDate());
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

    this->personEditButton->setEnabled(ids.count() > 0);
    this->personRemoveButton->setEnabled(ids.count() > 0);
    this->mergePersonsAction->setEnabled(ids.count() > 1);
    this->suggestPersonBirthAndDeathDatesAction->setEnabled(ids.count() > 0);

    emit this->selectionChanged(ids);
}

void PersonsListWidget::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
    QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());
    FPerson person(this->familyTree->findPerson(id));

    PersonEditDialog personEditDialog(this->familyTree,person,this);
    personEditDialog.exec();
}

void PersonsListWidget::onPersonAddButtonClicked()
{
    PersonAddDialog personAddDialog(this->familyTree,FPerson(),this);
    personAddDialog.exec();
}

void PersonsListWidget::onPersonEditButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    bool edit = true;

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to edit all selected persons?");
        int response = RMessageBox::question(this,tr("Remove person"),question);
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

void PersonsListWidget::onPersonRemoveButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->treeWidget->selectedItems();

    QString question;
    if (selectedItems.count() > 1)
    {
        question = tr("Are you sure you want to remove all selected persons?") + " (" + QLocale().toString(selectedItems.count()) + ")";
    }
    else
    {
        question = tr("Are you sure you want to remove selected person?");
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
    QTreeWidgetItem *item = new QTreeWidgetItem(this->treeWidget);
    PersonsListWidget::setTreeWidgetItemValues(item,person);
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
            return;
        }
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
            return;
        }
    }
}

void PersonsListWidget::onFileLoaded(const QString &)
{
    this->populate();
}

void PersonsListWidget::onShowDiagramDialog()
{
    foreach (auto selectedId, this->getSelectedIds())
    {
        DiagramDialog *diagramDialog = new DiagramDialog(this->familyTree,selectedId,this);
        diagramDialog->show();
        QObject::connect(diagramDialog,&DiagramDialog::personSelectionChanged,this,&PersonsListWidget::personDiagramSelectionChanged);
    }
}

void PersonsListWidget::onMergePersonsAction()
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

void PersonsListWidget::onSuggestPersonBirthAndDeathDatesAction()
{
    foreach (auto selectedId, this->getSelectedIds())
    {
        FPerson person(this->familyTree->findPerson(selectedId));
        PersonSuggetedDatesDialog personSuggestedDatesDialog(this->familyTree,person,this);
        personSuggestedDatesDialog.exec();
    }
}

void PersonsListWidget::personDiagramSelectionChanged(const QList<QUuid> &)
{

}
