#ifndef TREE_DIFF_UPDATE_RELATION_DIALOG_H
#define TREE_DIFF_UPDATE_RELATION_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include <rfl_relation.h>

#include <rgl_text_browser.h>

class TreeDiffUpdateRelationDialog : public QDialog
{
    Q_OBJECT

    protected:

        FRelation aRelation;
        FRelation bRelation;
        FRelation uRelation;

        RTextBrowser *uTextBrowser;

        QPushButton *updateButton;

    public:

        //! Constructor.
        explicit TreeDiffUpdateRelationDialog(const FRelation &aRelation, const FRelation &bRelation, QWidget *parent = nullptr);

        //! Return const reference to updated relation.
        const FRelation &getUpdatedRelation() const;

    private:

        //! Create text browser widget.
        static QWidget *createTextBrowser(QString title, const FRelation &relation, RTextBrowser **uTextBrowser = nullptr);

        //! Update U relation.
        void updateURelation(bool onlyMissing);

    private slots:

        //! Only missing check clicked.
        void onOnlyMissingChecked(bool clicked);

        //! U text has changed.
        void onUTextChanged();

};

#endif // TREE_DIFF_UPDATE_RELATION_DIALOG_H
