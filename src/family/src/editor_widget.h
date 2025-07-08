#ifndef EDITOR_WIDGET_H
#define EDITOR_WIDGET_H

#include <QWidget>

#include "session.h"
#include "persons_list_widget.h"
#include "relations_list_widget.h"
#include "person_details_widget.h"

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

    public:

        //! Constructor
        explicit EditorWidget(Session *session, QWidget *parent = nullptr);

    public slots:

        //! Persons list selection changed.
        void onPersonsSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        //! Relations list selection changed.
        void onRelationsSelectionChanged(const QList<QUuid> &selectedRelationsIds);

        //! Person changed.
        void onPersonChanged(const FPerson &person);

        //! Relation changed.
        void onRelationChanged(const FRelation &relation);

};

#endif // EDITOR_WIDGET_H
