#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QStringListModel>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>

#include "persons_list_edit_widget.h"

PersonsListEditWidget::PersonsListEditWidget(const FTree *familyTree,
                                             const QList<QUuid> &personsList,
                                             const QList<QUuid> &excludeList,
                                             QWidget *parent)
    : QWidget(parent)
    , familyTree(familyTree)
    , personsList(personsList)
    , excludeList(excludeList)
{
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->labelLayout = new RFlowLayout(0,0,0);
    mainLayout->addLayout(this->labelLayout);

    foreach (const QUuid &personId, this->personsList)
    {
        this->addLabel(personId);
    }

    QHBoxLayout *editLayout = new QHBoxLayout;
    editLayout->setSpacing(0);
    editLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(editLayout);

    this->lineEdit = new QLineEdit;
    this->lineEdit->setPlaceholderText(tr("Add person"));
    this->lineEdit->setClearButtonEnabled(false);
    this->lineEdit->setMinimumWidth(220);
    this->lineEdit->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    editLayout->addWidget(this->lineEdit);

    QCompleter *completer = new QCompleter;
    completer->setMaxVisibleItems(10);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    this->lineEdit->setCompleter(completer);
    this->updateLineEditCompleter();

    this->addButton = new QPushButton(addIcon,QString());
    this->addButton->setDisabled(true);
    this->addButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    editLayout->addWidget(this->addButton);

    QObject::connect(this->familyTree,&FTree::personAdded,this,&PersonsListEditWidget::onPersonAdded);
    QObject::connect(this->familyTree,&FTree::personChanged,this,&PersonsListEditWidget::onPersonChanged);
    QObject::connect(this->familyTree,&FTree::personRemoved,this,&PersonsListEditWidget::onPersonRemoved);
    QObject::connect(this->familyTree,&FTree::fileLoaded,this,&PersonsListEditWidget::onFileLoaded);

    QObject::connect(this->lineEdit,&QLineEdit::textEdited,this,&PersonsListEditWidget::onTextEdited);
    QObject::connect(this->addButton,&QPushButton::clicked,this,&PersonsListEditWidget::onAddClicked);
}

void PersonsListEditWidget::addLabel(const QUuid &personId)
{
    FPerson person = this->familyTree->findPerson(personId);

    QString label = PersonsListEditWidget::getPersonString(person);

    RLabelWidget *personLabel = new RLabelWidget(label,QVariant(personId),false);
    personLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    this->labelLayout->addWidget(personLabel);

    this->labelWidgets.insert(personId,personLabel);

    QObject::connect(personLabel,&RLabelWidget::closed,this,&PersonsListEditWidget::onCloseLabelClicked);
}

QString PersonsListEditWidget::getPersonString(const FPerson &person)
{
    QString personString = person.getName().toString();
    if (!person.getBirth().getDate().isEmpty())
    {
        personString += " (" + QLocale().toString(person.getBirth().getDate().toDate()) + ")";
    }
    return personString;
}

void PersonsListEditWidget::updateLineEditCompleter()
{
    QStringList personsStringList;
    foreach (const QUuid &personId, this->familyTree->getPersons())
    {
        FPerson person = this->familyTree->findPerson(personId);
        if (!this->personsList.contains(personId) && !this->excludeList.contains(personId))
        {
            personsStringList.append(PersonsListEditWidget::getPersonString(person));
        }
    }

    QStringListModel *model =  new QStringListModel(personsStringList);
    this->lineEdit->completer()->setModel(model);
}

void PersonsListEditWidget::onPersonAdded(const FPerson &)
{
    this->updateLineEditCompleter();
}

void PersonsListEditWidget::onPersonChanged(const FPerson &)
{
    this->updateLineEditCompleter();
}

void PersonsListEditWidget::onPersonRemoved(const QUuid &)
{
    this->updateLineEditCompleter();
}

void PersonsListEditWidget::onFileLoaded(const QString &)
{
    this->updateLineEditCompleter();
}

void PersonsListEditWidget::onCloseLabelClicked(const QVariant &data)
{
    QUuid personId = data.toUuid();

    if (this->labelWidgets.contains(personId))
    {
        delete this->labelWidgets.find(personId).value();
        for (int i=0;i<this->personsList.size();i++)
        {
            if (this->personsList[i] == personId)
            {
                this->personsList.remove(i);
                break;
            }
        }
        emit this->changed(this->personsList);
    }
}

void PersonsListEditWidget::onTextEdited(const QString &text)
{
    this->addButton->setDisabled(text.isEmpty());
}

void PersonsListEditWidget::onAddClicked()
{
    foreach (const QUuid &personId, this->familyTree->getPersons())
    {
        FPerson person = this->familyTree->findPerson(personId);
        if (!this->personsList.contains(personId))
        {
            if (PersonsListEditWidget::getPersonString(person) == this->lineEdit->text())
            {
                this->personsList.append(personId);
                this->addLabel(personId);
                this->lineEdit->clear();

                emit this->changed(this->personsList);

                return;
            }
        }
    }
}
