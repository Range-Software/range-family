#ifndef PERSON_SUGGESTED_DATES_DIALOG_H
#define PERSON_SUGGESTED_DATES_DIALOG_H

#include <QDialog>

#include <rfl_tree.h>

class PersonSuggestedDatesDialog
    : public QDialog
{
    Q_OBJECT

    protected:

        FTree *familyTree;
        FPerson person;

    public:

        //! Constructor.
        explicit PersonSuggestedDatesDialog(FTree *familyTree, const FPerson &rPerson, QWidget *parent = nullptr);

        //! Execute dialog.
        int exec();

};

#endif // PERSON_SUGGESTED_DATES_DIALOG_H
