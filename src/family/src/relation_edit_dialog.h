#ifndef RELATION_EDIT_DIALOG_H
#define RELATION_EDIT_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include "relation_edit_widget.h"

class RelationEditDialog : public QDialog
{
        Q_OBJECT

        protected:

            FTree *familyTree;
            FRelation relation;
            RelationEditWidget *relationEditWidget;
            QPushButton *editButton;

        public:

            //! Constructor.
            explicit RelationEditDialog(FTree *familyTree, const FRelation &relation, QWidget *parent = nullptr);

            //! Execute dialog.
            int exec();

    protected slots:

        void onRelationEdidWidgetChanged(const FRelation &);

};

#endif // RELATION_EDIT_DIALOG_H
