#ifndef PERSON_ADD_DIALOG_H
#define PERSON_ADD_DIALOG_H

#include <QDialog>

#include <rfl_tree.h>

#include "person_edit_widget.h"

class PersonAddDialog : public QDialog
{
    Q_OBJECT

    protected:

        FTree *familyTree;
        PersonEditWidget *personEditWidget;
        QUuid newPersonId;

    public:

        //! Constructor.
        explicit PersonAddDialog(FTree *familyTree, const FPerson &newPerson, QWidget *parent = nullptr);

        //! Execute dialog.
        int exec();

        //! Get person ID.
        const QUuid &getNewPersonId() const;

};

#endif // PERSON_ADD_DIALOG_H
