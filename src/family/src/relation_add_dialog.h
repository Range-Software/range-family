#ifndef RELATION_ADD_DIALOG_H
#define RELATION_ADD_DIALOG_H

#include <QDialog>

#include "relation_edit_widget.h"

class RelationAddDialog : public QDialog
{
        Q_OBJECT

        protected:

            FTree *familyTree;
            RelationEditWidget *relationEditWidget;
            QUuid newRelationId;

        public:

            //! Constructor.
            explicit RelationAddDialog(FTree *familyTree, const FRelation &newRelation, QWidget *parent = nullptr);

            //! Execute dialog.
            int exec();

            //! Get relation ID.
            const QUuid &getNewRelationId() const;

};

#endif // RELATION_ADD_DIALOG_H
