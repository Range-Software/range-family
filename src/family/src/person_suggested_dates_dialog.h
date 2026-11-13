#ifndef PERSON_SUGGESTED_DATES_DIALOG_H
#define PERSON_SUGGESTED_DATES_DIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QFormLayout>

#include <rfl_tree.h>

class PersonSuggestedDatesDialog
    : public QDialog
{
    Q_OBJECT

    protected:

        FTree *familyTree;
        FPerson person;
        FDateEstimate birth;
        FDateEstimate death;
        //! Check boxes are created only for dates which were estimated.
        QCheckBox *birthCheck;
        QCheckBox *deathCheck;

    private:

        //! Add row describing given estimate, return check box if the date can be applied.
        QCheckBox *addEstimateRow(QFormLayout *formLayout, const QString &label, const FDateEstimate &estimate, const FDate &recordedDate);

    public:

        //! Constructor.
        explicit PersonSuggestedDatesDialog(FTree *familyTree, const FPerson &rPerson, QWidget *parent = nullptr);

        //! Execute dialog.
        int exec();

};

#endif // PERSON_SUGGESTED_DATES_DIALOG_H
