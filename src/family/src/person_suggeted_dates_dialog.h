#ifndef PERSON_SUGGETED_DATES_DIALOG_H
#define PERSON_SUGGETED_DATES_DIALOG_H

#include <QDialog>

#include <rfl_tree.h>

class PersonSuggetedDatesDialog
    : public QDialog
{
    Q_OBJECT

    protected:

        FTree *familyTree;
        FPerson person;

    public:

        //! Constructor.
        explicit PersonSuggetedDatesDialog(FTree *familyTree, const FPerson &rPerson, QWidget *parent = nullptr);

        //! Execute dialog.
        int exec();

};

#endif // PERSON_SUGGETED_DATES_DIALOG_H
