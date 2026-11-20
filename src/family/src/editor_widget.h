#ifndef EDITOR_WIDGET_H
#define EDITOR_WIDGET_H

#include <QObject>

#include "session.h"
#include "diagram_widget.h"
#include "persons_list_widget.h"
#include "relations_list_widget.h"
#include "person_details_widget.h"

//! Editor of the family tree. It owns the views of the family tree and keeps them in sync,
//! the views are placed in the main window by its owner.
class EditorWidget : public QObject
{

    Q_OBJECT

    protected:

        //! Pointer to session.
        Session *session;
        //! Person details.
        PersonDetailsWidget *personDetailsList;
        //! Persons list.
        PersonsListWidget *personList;
        //! Relations list.
        RelationsListWidget *relationList;
        //! Family tree diagram.
        DiagramWidget *diagram;
        //! Diagram is being set from the lists.
        bool diagramUpdating;
        //! Relations were selected after persons, so the diagram follows the list of relations.
        bool relationsSelectedLast;

    public:

        //! Constructor
        explicit EditorWidget(Session *session, QObject *parent = nullptr);

        //! Return persons list.
        PersonsListWidget *getPersonsList() const;

        //! Return relations list.
        RelationsListWidget *getRelationsList() const;

        //! Return person details.
        PersonDetailsWidget *getPersonDetails() const;

        //! Return family tree diagram.
        DiagramWidget *getDiagram() const;

    public slots:

        //! Persons list selection changed.
        void onPersonsSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        //! Relations list selection changed.
        void onRelationsSelectionChanged(const QList<QUuid> &selectedRelationsIds);

        //! Person changed.
        void onPersonChanged(const FPerson &person);

        //! Relation changed.
        void onRelationChanged(const FRelation &relation);

        //! Show family tree of the person or relation selected last in the diagram, if the diagram is visible.
        //! Without a single selected item the diagram is kept, an empty one shows the active person.
        void updateDiagram();

    protected:

        //! Update views depending on selected persons.
        void updatePersonsSelection(const QList<QUuid> &selectedPersonsIds);

        //! Update views depending on selected relations.
        void updateRelationsSelection(const QList<QUuid> &selectedRelationsIds);

        //! Update the diagram when it gets shown.
        bool eventFilter(QObject *watched, QEvent *event) override;

    protected slots:

        //! Person or relation the diagram is built around has changed.
        void onDiagramItemIdChanged(const QUuid &itemId);

};

#endif // EDITOR_WIDGET_H
