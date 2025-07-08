#ifndef PERSON_EDIT_DIALOG_H
#define PERSON_EDIT_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include <rfl_tree.h>

#include "person_edit_widget.h"

class PersonEditDialog : public QDialog
{
    Q_OBJECT

    protected:

        FTree *familyTree;
        FPerson person;
        PersonEditWidget *personEditWidget;
        QPushButton *editButton;

    public:

        //! Constructor.
        explicit PersonEditDialog(FTree *familyTree, const FPerson &person, QWidget *parent = nullptr);

        //! Execute dialog.
        int exec();

    protected slots:

        void onPersonEdidWidgetChanged(const FPerson &);

};

#endif // PERSON_EDIT_DIALOG_H
