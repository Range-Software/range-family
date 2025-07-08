#ifndef TREE_DIFF_DIALOG_H
#define TREE_DIFF_DIALOG_H

#include <QDialog>

#include <rfl_tree.h>

class TreeDiffDialog : public QDialog
{

    Q_OBJECT

    protected:

        bool bTreeSaved;

    public:

        //! Constructor.
        explicit TreeDiffDialog(const QString &aFileName, const QString &bFileName, bool easyMergeFirst, QWidget *parent = nullptr);

    private slots:

        //! Dialog accepted signal.
        void onAccepted();

        //! B tree has changed.
        void onBTreeChanged();

        //! B tree has been saved.
        void onBTreeSaved();

};

#endif // TREE_DIFF_DIALOG_H
