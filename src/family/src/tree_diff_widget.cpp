#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QJsonDocument>
#include <QFileInfo>
#include <QFileDialog>
#include <QTimer>

#include <rbl_logger.h>
#include <rbl_error.h>
#include <rgl_file_chooser_button.h>
#include <rgl_message_box.h>

#include "application.h"
#include "tree_diff_widget.h"
#include "tree_diff_update_person_dialog.h"
#include "tree_diff_update_relation_dialog.h"

TreeDiffWidget::TreeDiffWidget(const QString &aFileName, const QString &bFileName, bool easyMergeFirst, QWidget *parent)
    : QWidget{parent}
    , aFileName{aFileName}
    , bFileName{bFileName}
    , easyMergeFirst{easyMergeFirst}
    , nImportedPersons{0}
    , nImportedRelations{0}
{
    this->aTree = new FTree(this);
    this->bTree = new FTree(this);

    if (QFileInfo::exists(this->aFileName))
    {
        this->loadATreeFile();
    }
    if (QFileInfo::exists(this->bFileName))
    {
        this->loadBTreeFile();
    }

    if (easyMergeFirst)
    {
        this->bTree->importMissing(this->aTree,this->nImportedPersons,this->nImportedRelations);

        RLogger::info("Number of imported persons: %u\n",this->nImportedPersons);
        RLogger::info("Number of imported relations: %u\n",this->nImportedRelations);
    }

    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon removeIcon(":/icons/action/pixmaps/range-remove.svg");
    QIcon iconUpdate(":/icons/action/pixmaps/range-edit.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    splitter->addWidget(this->createTreeWidget(FTreeDiff::A));
    splitter->addWidget(this->createTreeWidget(FTreeDiff::B));

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    mainLayout->addLayout(hBoxLayout);

    hBoxLayout->addStretch(1);

    this->addButton = new QPushButton(addIcon,"Add");
    hBoxLayout->addWidget(this->addButton);

    this->updateButton = new QPushButton(iconUpdate,"Update");
    hBoxLayout->addWidget(this->updateButton);

    this->removeButton = new QPushButton(removeIcon,"Remove");
    hBoxLayout->addWidget(this->removeButton);

    hBoxLayout->addStretch(1);

    QObject::connect(this->aTreeWidget,&QTreeWidget::itemSelectionChanged,this,&TreeDiffWidget::onATreeItemSelectionChanged);
    QObject::connect(this->bTreeWidget,&QTreeWidget::itemSelectionChanged,this,&TreeDiffWidget::onBTreeItemSelectionChanged);

    QObject::connect(this->addButton,&QPushButton::clicked,this,&TreeDiffWidget::onAddClicked);
    QObject::connect(this->removeButton,&QPushButton::clicked,this,&TreeDiffWidget::onRemoveClicked);
    QObject::connect(this->updateButton,&QPushButton::clicked,this,&TreeDiffWidget::onUpdateClicked);

    if (easyMergeFirst && this->treeDiff.containsDifferences())
    {
        this->saveButton->setEnabled(true);
    }

    QTimer::singleShot(200, this, &TreeDiffWidget::startupCheck);
}

void TreeDiffWidget::loadATreeFile()
{
    try
    {
        this->aTree->readFile(this->aFileName);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to read Family tree XML file \"%s\". %s\n",
                       aFileName.toUtf8().constData(),
                       error.getMessage().toUtf8().constData());
    }
}

void TreeDiffWidget::loadBTreeFile()
{
    try
    {
        this->bTree->readFile(this->bFileName);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to read Family tree XML file \"%s\". %s\n",
                       bFileName.toUtf8().constData(),
                       error.getMessage().toUtf8().constData());
    }
}

QWidget *TreeDiffWidget::createTreeWidget(FTreeDiff::Side side)
{
    QWidget *widget = new QWidget;

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    widget->setLayout(vBoxLayout);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);

    RFileChooserButton *fileChooser = new RFileChooserButton(QString(side == FTreeDiff::A ? "A" : "B") + " " + tr("file"),
                                                             RFileChooserButton::OpenFile,
                                                             QString(side == FTreeDiff::A ? "A" : "B") + " " + tr("file"),
                                                             side == FTreeDiff::A ? this->aFileName : this->bFileName,
                                                             tr("Family files")+ "(*.xml *.json)");
    fileChooser->setSearchDirectory(Application::instance()->getApplicationSettings()->getDataDir());
    hBoxLayout->addWidget(fileChooser);

    hBoxLayout->addStretch(1);

    if (side == FTreeDiff::B)
    {
        this->saveButton = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave),tr("Save changes"));
        this->saveButton->setDisabled(true);
        hBoxLayout->addWidget(this->saveButton);

        QObject::connect(this->saveButton,&QPushButton::clicked,this,&TreeDiffWidget::onSaveButtonClicked);
    }

    QList<RTreeWidget::ColumnInfoItem> columnInfo;
    for (int type=ColumnState;type<NunberOfColumns;type++)
    {
        RTreeWidget::ColumnInfoItem columnInfoItem;
        columnInfoItem.column = type;
        columnInfoItem.title = TreeDiffWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = false;
        columnInfoItem.available = true;
        columnInfo.append(columnInfoItem);
    }

    RTreeWidget *treeWidget = new RTreeWidget(columnInfo);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->setAlternatingRowColors(true);
    vBoxLayout->addWidget(treeWidget);

    this->populateTreeWidget(treeWidget, side);

    if (side == FTreeDiff::A)
    {
        this->aTreeWidget = treeWidget;
    }
    else
    {
        this->bTreeWidget = treeWidget;
    }

    if (side == FTreeDiff::A)
    {
        QObject::connect(fileChooser,&RFileChooserButton::fileNameChanged,this,&TreeDiffWidget::onAFileNameChanged);
    }
    else
    {
        QObject::connect(fileChooser,&RFileChooserButton::fileNameChanged,this,&TreeDiffWidget::onBFileNameChanged);
    }

    return widget;
}

void TreeDiffWidget::populateTreeWidget(QTreeWidget *treeWidget, FTreeDiff::Side side)
{
    QIcon iconAdd(":/icons/action/pixmaps/range-add.svg");
    QIcon iconRemove(":/icons/action/pixmaps/range-remove.svg");
    QIcon iconUpdate(":/icons/action/pixmaps/range-edit.svg");

    treeWidget->clear();

    FTree *pTree = (side == FTreeDiff::A) ? this->aTree : this->bTree;

    const QSet<QUuid> &personsAdd = (side == FTreeDiff::A) ? this->treeDiff.getPersons().a : this->treeDiff.getPersons().b;
    const QSet<QUuid> &personsDiff = this->treeDiff.getPersons().ab;

    if (personsAdd.size() || personsDiff.size())
    {
        for (const QUuid &id : personsAdd)
        {
            FPerson person = pTree->findPerson(id);
            if (!person.isEmpty())
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
                item->setIcon(ColumnState,(side == FTreeDiff::A) ? iconAdd : iconRemove);
                item->setText(ColumnId,id.toString(QUuid::WithoutBraces));
                item->setText(ColumnItemType,tr("Person"));
                item->setText(ColumnText,person.toShortString());
            }
        }

        for (const QUuid &id : personsDiff)
        {
            FPerson person = pTree->findPerson(id);
            if (!person.isEmpty())
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
                item->setIcon(ColumnState,iconUpdate);
                item->setText(ColumnId,id.toString(QUuid::WithoutBraces));
                item->setText(ColumnItemType,tr("Person"));
                item->setText(ColumnText,person.toShortString());
            }
        }
    }

    const QSet<QUuid> &relationsAdd = (side == FTreeDiff::A) ? this->treeDiff.getRelations().a : this->treeDiff.getRelations().b;
    const QSet<QUuid> &relationsDiff = this->treeDiff.getRelations().ab;

    if (relationsAdd.size() || relationsDiff.size())
    {
        for (const QUuid &id : relationsAdd)
        {
            FRelation relation = pTree->findRelation(id);
            if (!relation.isEmpty())
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
                item->setIcon(ColumnState,(side == FTreeDiff::A) ? iconAdd : iconRemove);
                item->setText(ColumnId,id.toString(QUuid::WithoutBraces));
                item->setText(ColumnItemType,tr("Relation"));
                item->setText(ColumnText,relation.toShortString());
            }
        }

        for (const QUuid &id : relationsDiff)
        {
            FRelation relation = pTree->findRelation(id);
            if (!relation.isEmpty())
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
                item->setIcon(ColumnState,iconUpdate);
                item->setText(ColumnId,id.toString(QUuid::WithoutBraces));
                item->setText(ColumnItemType,tr("Relation"));
                item->setText(ColumnText,relation.toShortString());
            }
        }
    }

    treeWidget->resizeColumnToContents(ColumnState);
    treeWidget->resizeColumnToContents(ColumnItemType);
    // treeWidget->resizeColumnToContents(ColumnId);
}

void TreeDiffWidget::removeId(const QUuid &id)
{
    this->treeDiff.getPersons().a.remove(id);
    this->treeDiff.getPersons().b.remove(id);
    this->treeDiff.getPersons().ab.remove(id);

    for (int i=this->aTreeWidget->topLevelItemCount()-1;i>=0;i--)
    {
        QTreeWidgetItem *item = this->aTreeWidget->topLevelItem(i);
        if (QUuid(item->text(ColumnId)) == id)
        {
            delete this->aTreeWidget->takeTopLevelItem(i);
        }
    }

    for (int i=this->bTreeWidget->topLevelItemCount()-1;i>=0;i--)
    {
        QTreeWidgetItem *item = this->bTreeWidget->topLevelItem(i);
        if (QUuid(item->text(ColumnId)) == id)
        {
            delete this->bTreeWidget->takeTopLevelItem(i);
        }
    }
}

QString TreeDiffWidget::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnState:            return QString();
        case ColumnId:               return tr("ID");
        case ColumnItemType:         return tr("Type");
        case ColumnText:             return tr("Text");
        default:                     return QString();
    }
}

void TreeDiffWidget::checkIfTreeAreSame()
{
    if (!this->treeDiff.containsDifferences())
    {
        RMessageBox::information(this,tr("Trees are equal"),tr("There are no differences between A and B family trees."));
    }
}

void TreeDiffWidget::onAFileNameChanged(const QString &fileName)
{
    this->aFileName = fileName;
    this->loadATreeFile();
    if (QFileInfo::exists(this->aFileName) && QFileInfo::exists(this->bFileName))
    {
        this->treeDiff = FTree::diffFiles(this->aFileName,this->bFileName);
        this->populateTreeWidget(this->aTreeWidget, FTreeDiff::A);
        this->populateTreeWidget(this->bTreeWidget, FTreeDiff::B);

        this->checkIfTreeAreSame();
    }
}

void TreeDiffWidget::onBFileNameChanged(const QString &fileName)
{
    this->bFileName = fileName;
    this->loadBTreeFile();
    if (QFileInfo::exists(this->aFileName) && QFileInfo::exists(this->bFileName))
    {
        this->treeDiff = FTree::diffFiles(this->aFileName,this->bFileName);
        this->populateTreeWidget(this->aTreeWidget, FTreeDiff::A);
        this->populateTreeWidget(this->bTreeWidget, FTreeDiff::B);

        this->checkIfTreeAreSame();
    }
}

void TreeDiffWidget::onATreeItemSelectionChanged()
{
    bool addEnabled = false;
    bool removeEnabled = false;
    bool updateEnabled = false;

    bool areSignalsBlocked = this->bTreeWidget->signalsBlocked();
    this->bTreeWidget->blockSignals(true);

    this->bTreeWidget->clearSelection();

    QList<QTreeWidgetItem*> selectedItems = this->aTreeWidget->selectedItems();
    if (selectedItems.size())
    {
        QUuid id(selectedItems.at(0)->text(ColumnId));

        // Persons
        if (this->treeDiff.getPersons().a.contains(id))
        {
            addEnabled = true;
            this->bTreeWidget->clearSelection();
        }

        if (this->treeDiff.getPersons().ab.contains(id))
        {
            for (int i=0;i<this->bTreeWidget->topLevelItemCount();i++)
            {
                QTreeWidgetItem *item = this->bTreeWidget->topLevelItem(i);
                if (QUuid(item->text(ColumnId)) == id)
                {
                    updateEnabled = true;
                    item->setSelected(true);
                }
            }
        }

        // Relations
        if (this->treeDiff.getRelations().a.contains(id))
        {
            addEnabled = true;
            this->bTreeWidget->clearSelection();
        }

        if (this->treeDiff.getRelations().ab.contains(id))
        {
            for (int i=0;i<this->bTreeWidget->topLevelItemCount();i++)
            {
                QTreeWidgetItem *item = this->bTreeWidget->topLevelItem(i);
                if (QUuid(item->text(ColumnId)) == id)
                {
                    updateEnabled = true;
                    item->setSelected(true);
                }
            }
        }
    }

    this->bTreeWidget->blockSignals(areSignalsBlocked);

    this->addButton->setEnabled(addEnabled);
    this->removeButton->setEnabled(removeEnabled);
    this->updateButton->setEnabled(updateEnabled);
}

void TreeDiffWidget::onBTreeItemSelectionChanged()
{
    bool addEnabled = false;
    bool removeEnabled = false;
    bool updateEnabled = false;

    bool areSignalsBlocked = this->aTreeWidget->signalsBlocked();
    this->aTreeWidget->blockSignals(true);

    this->aTreeWidget->clearSelection();

    QList<QTreeWidgetItem*> selectedItems = this->bTreeWidget->selectedItems();
    if (selectedItems.size())
    {
        QUuid id(selectedItems.at(0)->text(ColumnId));

        // Persons
        if (this->treeDiff.getPersons().b.contains(id))
        {
            removeEnabled = true;
            this->aTreeWidget->clearSelection();
        }

        if (this->treeDiff.getPersons().ab.contains(id))
        {
            for (int i=0;i<this->aTreeWidget->topLevelItemCount();i++)
            {
                QTreeWidgetItem *item = this->aTreeWidget->topLevelItem(i);
                if (QUuid(item->text(ColumnId)) == id)
                {
                    updateEnabled = true;
                    item->setSelected(true);
                }
            }
        }

        // Relations
        if (this->treeDiff.getRelations().b.contains(id))
        {
            removeEnabled = true;
            this->aTreeWidget->clearSelection();
        }

        if (this->treeDiff.getRelations().ab.contains(id))
        {
            for (int i=0;i<this->aTreeWidget->topLevelItemCount();i++)
            {
                QTreeWidgetItem *item = this->aTreeWidget->topLevelItem(i);
                if (QUuid(item->text(ColumnId)) == id)
                {
                    updateEnabled = true;
                    item->setSelected(true);
                }
            }
        }
    }

    this->aTreeWidget->blockSignals(areSignalsBlocked);

    this->addButton->setEnabled(addEnabled);
    this->removeButton->setEnabled(removeEnabled);
    this->updateButton->setEnabled(updateEnabled);
}

void TreeDiffWidget::onAddClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->aTreeWidget->selectedItems();
    if (selectedItems.size())
    {
        QUuid id(selectedItems.at(0)->text(ColumnId));

        // Persons
        if (this->treeDiff.getPersons().a.contains(id))
        {
            QString question = tr("Add person from tree A to tree B") + "<pre>" + QJsonDocument(this->aTree->findPerson(id).toJson()).toJson() + "</pre>";
            if (RMessageBox::question(this,tr("Add person"),question) == RMessageBox::Yes)
            {
                this->bTree->addPerson(this->aTree->findPerson(id));
                this->removeId(id);

                this->saveButton->setEnabled(true);
                emit this->bTreeChanged();
            }
        }

        // Relations
        if (this->treeDiff.getRelations().a.contains(id))
        {
            QString question = tr("Add relation from tree A to tree B") + "<pre>" + QJsonDocument(this->aTree->findRelation(id).toJson()).toJson() + "</pre>";
            if (RMessageBox::question(this,tr("Add relation"),question) == RMessageBox::Yes)
            {
                this->bTree->addRelation(this->aTree->findRelation(id));
                this->removeId(id);

                this->saveButton->setEnabled(true);
                emit this->bTreeChanged();
            }
        }
    }
}

void TreeDiffWidget::onRemoveClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->bTreeWidget->selectedItems();
    if (selectedItems.size())
    {
        QUuid id(selectedItems.at(0)->text(ColumnId));

        // Persons
        if (this->treeDiff.getPersons().b.contains(id))
        {
            QString question = tr("Remove person from tree B") + "<pre>" + QJsonDocument(this->bTree->findPerson(id).toJson()).toJson() + "</pre>";
            if (RMessageBox::question(this,tr("Remove person"),question) == RMessageBox::Yes)
            {
                try
                {
                    this->bTree->removePerson(id,true);
                    this->removeId(id);

                    this->treeDiff = FTree::diffFiles(this->aFileName,this->bFileName);
                    this->populateTreeWidget(this->aTreeWidget, FTreeDiff::A);
                    this->populateTreeWidget(this->bTreeWidget, FTreeDiff::B);

                    this->saveButton->setEnabled(true);
                    emit this->bTreeChanged();
                }
                catch (const RError &error)
                {
                    RLogger::error("Failed to remove person from B tree. %s\n",error.getMessage().toUtf8().constData());
                }
            }
        }

        // Relations
        if (this->treeDiff.getRelations().b.contains(id))
        {
            QString question = tr("Remove relation from tree B") + "<pre>" + QJsonDocument(this->bTree->findRelation(id).toJson()).toJson() + "</pre>";
            if (RMessageBox::question(this,tr("Remove relation"),question) == RMessageBox::Yes)
            {
                try
                {
                    this->bTree->removeRelation(id);
                    this->removeId(id);

                    this->saveButton->setEnabled(true);
                    emit this->bTreeChanged();
                }
                catch (const RError &error)
                {
                    RLogger::error("Failed to remove relation from B tree. %s\n",error.getMessage().toUtf8().constData());
                }
            }
        }
    }
}

void TreeDiffWidget::onUpdateClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->bTreeWidget->selectedItems();
    if (selectedItems.size())
    {
        QUuid id(selectedItems.at(0)->text(ColumnId));

        // Persons
        if (this->treeDiff.getPersons().ab.contains(id))
        {
            for (int i=0;i<this->aTreeWidget->topLevelItemCount();i++)
            {
                QTreeWidgetItem *item = this->aTreeWidget->topLevelItem(i);
                if (QUuid(item->text(ColumnId)) == id)
                {
                    FPerson aPerson = this->aTree->findPerson(id);
                    FPerson bPerson = this->bTree->findPerson(id);
                    TreeDiffUpdatePersonDialog dialog(aPerson,bPerson,this);
                    if (dialog.exec() == QDialog::Accepted)
                    {
                        const FPerson &uPerson = dialog.getUpdatedPerson();
                        this->bTree->setPerson(uPerson);

                        if (bPerson == uPerson)
                        {
                            this->removeId(id);

                            this->saveButton->setEnabled(true);
                            emit this->bTreeChanged();
                        }
                    }
                }
            }
        }

        // Relations
        if (this->treeDiff.getRelations().ab.contains(id))
        {
            for (int i=0;i<this->aTreeWidget->topLevelItemCount();i++)
            {
                QTreeWidgetItem *item = this->aTreeWidget->topLevelItem(i);
                if (QUuid(item->text(ColumnId)) == id)
                {
                    FRelation aRelation = this->aTree->findRelation(id);
                    FRelation bRelation = this->bTree->findRelation(id);
                    TreeDiffUpdateRelationDialog dialog(aRelation,bRelation,this);
                    if (dialog.exec() == QDialog::Accepted)
                    {
                        const FRelation &uRelation = dialog.getUpdatedRelation();
                        this->bTree->setRelation(uRelation);

                        if (bRelation == uRelation)
                        {
                            this->removeId(id);

                            this->saveButton->setEnabled(true);
                            emit this->bTreeChanged();
                        }
                    }
                }
            }
        }
    }
}

void TreeDiffWidget::onSaveButtonClicked()
{
    QString fileName = Application::instance()->getSession()->getTreeFileName();

    if (fileName.isEmpty())
    {
        fileName = Application::instance()->getApplicationSettings()->getDataDir() + "/tree.xml";
    }

    fileName = QFileDialog::getSaveFileName(Application::instance()->getMainWindow(),
                                            "Save tree as",
                                            fileName,
                                            "Family tree files (*.xml *.json);;Any files (*)",
                                            nullptr,
                                            QFileDialog::DontUseNativeDialog);

    if (!fileName.isEmpty())
    {
        try
        {
            this->bTree->writeFile(fileName);
            RLogger::info("Tree has been successfully saved to file \'%s\'.\n", fileName.toUtf8().constData());
            this->saveButton->setDisabled(true);
            emit this->bTreeSaved();
        }
        catch (const RError &error)
        {
            RLogger::error("Failed to save tree into file \'%s\'. %s\n", error.getMessage().toUtf8().constData());
        }
    }
}

void TreeDiffWidget::startupCheck()
{
    if (this->easyMergeFirst)
    {
        QString importMessage = QString("<ul><li>%1: <b>%2</b></li><li>%3: <b>%4</b></li></ul>").arg(tr("Number of imported persons"),QLocale().toString(nImportedPersons),tr("Number of imported relations"),QLocale().toString(nImportedRelations));
        RMessageBox::information(this,tr("Tree import"),importMessage);
    }

    if (!this->aFileName.isEmpty() && !this->bFileName.isEmpty())
    {
        this->checkIfTreeAreSame();
    }
}
