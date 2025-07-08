#include <QSplitter>
#include <QVBoxLayout>

#include "editor_widget.h"

EditorWidget::EditorWidget(Session *session, QWidget *parent)
    : QWidget{parent}
    , session(session)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *horizontalSplitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(horizontalSplitter);

    this->personList = new PersonsListWidget(this->session->getTree());
    horizontalSplitter->addWidget(this->personList);

    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical);
    horizontalSplitter->addWidget(verticalSplitter);

    this->personDetailsList = new PersonDetailsWidget(this->session->getTree());
    verticalSplitter->addWidget(this->personDetailsList);

    this->relationList = new RelationsListWidget(this->session->getTree());
    verticalSplitter->addWidget(this->relationList);

    QObject::connect(this->personList,&PersonsListWidget::selectionChanged,this,&EditorWidget::onPersonsSelectionChanged);
    QObject::connect(this->relationList,&RelationsListWidget::selectionChanged,this,&EditorWidget::onRelationsSelectionChanged);
    QObject::connect(this->session->getTree(),&FTree::personChanged,this,&EditorWidget::onPersonChanged);
    QObject::connect(this->session->getTree(),&FTree::relationChanged,this,&EditorWidget::onRelationChanged);
}

void EditorWidget::onPersonsSelectionChanged(const QList<QUuid> &selectedPersonsIds)
{
    QList<QUuid> relationsIds;
    foreach (const QUuid &personId, selectedPersonsIds)
    {
        QList<QUuid> personRelationsIds = this->session->getTree()->findPersonRelations(personId);
        foreach (const QUuid &relationId, personRelationsIds)
        {
            relationsIds.append(relationId);
        }
    }
    this->relationList->highlightRelations(relationsIds);
    this->personDetailsList->setPersons(selectedPersonsIds);
}

void EditorWidget::onRelationsSelectionChanged(const QList<QUuid> &selectedRelationsIds)
{
    QList<QUuid> personsIds;
    foreach (const QUuid &relationId, selectedRelationsIds)
    {
        QList<QUuid> relationPersonsIds = this->session->getTree()->findRelationPersons(relationId);
        foreach (const QUuid &personId, relationPersonsIds)
        {
            personsIds.append(personId);
        }
    }
    this->personList->highlightPersons(personsIds);
}

void EditorWidget::onPersonChanged(const FPerson &person)
{
    QList<QUuid> selectedIds = this->personList->getSelectedIds();
    if (selectedIds.contains(person.getId()))
    {
        this->onPersonsSelectionChanged(selectedIds);
    }
}

void EditorWidget::onRelationChanged(const FRelation &relation)
{
    QList<QUuid> selectedIds = this->relationList->getSelectedIds();
    if (selectedIds.contains(relation.getId()))
    {
        this->onRelationsSelectionChanged(selectedIds);
    }
}
