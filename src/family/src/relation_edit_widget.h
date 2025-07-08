#ifndef RELATION_EDIT_WIDGET_H
#define RELATION_EDIT_WIDGET_H

#include <QWidget>

#include <rfl_tree.h>
#include <rfl_relation.h>

#include "persons_list_edit_widget.h"
#include "event_edit_widget.h"

class RelationEditWidget : public QWidget
{
    Q_OBJECT

    protected:

        const FTree *familyTree;
        FRelation relation;

        PersonsListEditWidget *partnersEdit;
        PersonsListEditWidget *childrenEdit;
        EventEditWidget *beginEdit;
        EventEditWidget *endEdit;

    public:

        //! Constructor.
        explicit RelationEditWidget(const FTree *familyTree, const FRelation &relation, QWidget *parent = nullptr);

        //! Return const reference to relation.
        const FRelation &getRelation() const;

    protected slots:

        void onPartnersListChanged(const QList<QUuid> &partnersList);
        void onChildrenListChanged(const QList<QUuid> &childrenList);
        void onBeginEventChanged(const FEvent &event);
        void onEndEventChanged(const FEvent &event);

    signals:

        void changed(const FRelation &relation);

};

#endif // RELATION_EDIT_WIDGET_H
