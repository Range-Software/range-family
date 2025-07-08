#ifndef TREE_DIFF_WIDGET_H
#define TREE_DIFF_WIDGET_H

#include <QPushButton>

#include <rfl_tree.h>

#include <rgl_tree_widget.h>

class TreeDiffWidget : public QWidget
{

    Q_OBJECT

    protected:

        enum ColumnType
        {
            ColumnState = 0,
            ColumnItemType,
            ColumnId,
            ColumnText,
            NunberOfColumns
        };

    protected:

        QString aFileName;
        QString bFileName;
        FTreeDiff treeDiff;

        FTree *aTree;
        FTree *bTree;

        bool easyMergeFirst;
        uint nImportedPersons;
        uint nImportedRelations;

        RTreeWidget *aTreeWidget;
        RTreeWidget *bTreeWidget;

        QPushButton *saveButton;

        QPushButton *updateButton;
        QPushButton *addButton;
        QPushButton *removeButton;

    public:

        //! Constructor.
        explicit TreeDiffWidget(const QString &aFileName, const QString &bFileName, bool easyMergeFirst, QWidget *parent = nullptr);

    private:

        //! Load A tree files.
        void loadATreeFile();

        //! Load B tree files.
        void loadBTreeFile();

        //! Create tree widget.
        QWidget *createTreeWidget(FTreeDiff::Side side);

        //! Populate list of persons.
        void populateTreeWidget(QTreeWidget *treeWidget, FTreeDiff::Side side);

        //! Remove ID from trees.
        void removeId(const QUuid &id);

        //! Return column name string.
        static QString getColumnName(ColumnType columnType);

        //! Check if trees are same.
        void checkIfTreeAreSame();

    private slots:

        //! A filename has changed.
        void onAFileNameChanged(const QString &fileName);

        //! B filename has changed.
        void onBFileNameChanged(const QString &fileName);

        //! A tree item selection has changed.
        void onATreeItemSelectionChanged();

        //! B tree item selection has changed.
        void onBTreeItemSelectionChanged();

        //! Add button has been clicked.
        void onAddClicked();

        //! Remove button has been clicked.
        void onRemoveClicked();

        //! Edit button has been clicked.
        void onUpdateClicked();

        //! Save butto has been clicked.
        void onSaveButtonClicked();

        //! Check if trees are same.
        void startupCheck();

    signals:

        void bTreeChanged();

        void bTreeSaved();

};

#endif // TREE_DIFF_WIDGET_H
