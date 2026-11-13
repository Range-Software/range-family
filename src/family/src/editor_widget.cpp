#include <QVBoxLayout>

#include "editor_widget.h"

EditorWidget::EditorWidget(Session *session, QWidget *parent)
    : QWidget{parent}
    , session(session)
    , diagramUpdating(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->personList = new PersonsListWidget(this->session->getTree());
    mainLayout->addWidget(this->personList);

    // Remaining views are placed by the owner of the editor.
    this->personDetailsList = new PersonDetailsWidget(this->session->getTree(),this);
    this->relationList = new RelationsListWidget(this->session->getTree(),this);
    this->diagram = new DiagramWidget(this->session->getTree(),QUuid(),this);

    QObject::connect(this->personList,&PersonsListWidget::selectionChanged,this,&EditorWidget::onPersonsSelectionChanged);
    QObject::connect(this->session,&Session::activePersonIdChanged,this->personList,&PersonsListWidget::onActivePersonIdChanged);
    QObject::connect(this->relationList,&RelationsListWidget::selectionChanged,this,&EditorWidget::onRelationsSelectionChanged);
    QObject::connect(this->personDetailsList,&PersonDetailsWidget::personActivated,this->personList,&PersonsListWidget::selectPerson);
    QObject::connect(this->session->getTree(),&FTree::personChanged,this,&EditorWidget::onPersonChanged);
    QObject::connect(this->session->getTree(),&FTree::relationChanged,this,&EditorWidget::onRelationChanged);

    QObject::connect(this->diagram,&DiagramWidget::itemIdChanged,this,&EditorWidget::onDiagramItemIdChanged);
    QObject::connect(this->session,&Session::activePersonIdChanged,this,&EditorWidget::updateDiagram);
    // Queued, so the lists are populated with the loaded tree first.
    QObject::connect(this->session->getTree(),&FTree::fileLoaded,this,&EditorWidget::updateDiagram,Qt::QueuedConnection);
}

PersonsListWidget *EditorWidget::getPersonsList() const
{
    return this->personList;
}

RelationsListWidget *EditorWidget::getRelationsList() const
{
    return this->relationList;
}

PersonDetailsWidget *EditorWidget::getPersonDetails() const
{
    return this->personDetailsList;
}

DiagramWidget *EditorWidget::getDiagram() const
{
    return this->diagram;
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
    this->updateDiagram();
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

void EditorWidget::updateDiagram()
{
    // Building of a diagram nobody sees would only slow down the selection.
    if (!this->diagram->isVisible())
    {
        return;
    }

    const FTree *tree = this->session->getTree();

    QUuid itemId;
    const QList<QUuid> selectedIds = this->personList->getSelectedIds();
    if (selectedIds.count() == 1)
    {
        itemId = selectedIds.first();
    }
    else if (!tree->containsPerson(this->diagram->getItemId()) && !tree->containsRelation(this->diagram->getItemId()))
    {
        itemId = this->session->getActivePersonId();
    }

    if (!itemId.isNull())
    {
        this->diagramUpdating = true;
        this->diagram->setItemId(itemId);
        this->diagramUpdating = false;
    }
}

void EditorWidget::onDiagramItemIdChanged(const QUuid &itemId)
{
    // Only navigation in the diagram itself selects in the lists.
    if (this->diagramUpdating)
    {
        return;
    }

    if (this->session->getTree()->containsPerson(itemId))
    {
        this->personList->selectPerson(itemId);
    }
    else if (this->session->getTree()->containsRelation(itemId))
    {
        this->relationList->selectRelation(itemId);
    }
}
