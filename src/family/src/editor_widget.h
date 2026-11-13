#ifndef EDITOR_WIDGET_H
#define EDITOR_WIDGET_H

#include <QWidget>

#include "session.h"
#include "diagram_widget.h"
#include "persons_list_widget.h"
#include "relations_list_widget.h"
#include "person_details_widget.h"

//! Editor of the family tree. It shows the list of persons itself and keeps it in sync
//! with the other views, which are placed in the main window by its owner.
class EditorWidget : public QWidget
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
        //! Diagram is being set from the list of persons.
        bool diagramUpdating;

    public:

        //! Constructor
        explicit EditorWidget(Session *session, QWidget *parent = nullptr);

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

        //! Show family tree of the selected person in the diagram, if the diagram is visible.
        //! Without a selected person the diagram is kept, an empty one shows the active person.
        void updateDiagram();

    protected slots:

        //! Person or relation the diagram is built around has changed.
        void onDiagramItemIdChanged(const QUuid &itemId);

};

#endif // EDITOR_WIDGET_H
