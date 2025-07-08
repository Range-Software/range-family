#ifndef TREE_DIFF_UPDATE_PERSON_DIALOG_H
#define TREE_DIFF_UPDATE_PERSON_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include <rfl_person.h>

#include <rgl_text_browser.h>

class TreeDiffUpdatePersonDialog : public QDialog
{
    Q_OBJECT

    protected:

        FPerson aPerson;
        FPerson bPerson;
        FPerson uPerson;

        RTextBrowser *uTextBrowser;

        QPushButton *updateButton;

    public:

        //! Constructor.
        explicit TreeDiffUpdatePersonDialog(const FPerson &aPerson, const FPerson &bPerson, QWidget *parent = nullptr);

        //! Return const reference to updated person.
        const FPerson &getUpdatedPerson() const;

    private:

        //! Create text browser widget.
        static QWidget *createTextBrowser(QString title, const FPerson &person, RTextBrowser **uTextBrowser = nullptr);

        //! Update U relation.
        void updateUPerson(bool onlyMissing);

    private slots:

        //! Only missing check clicked.
        void onOnlyMissingChecked(bool clicked);

        //! U text has changed.
        void onUTextChanged();

};

#endif // TREE_DIFF_UPDATE_PERSON_DIALOG_H
