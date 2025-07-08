#ifndef PERSONS_LIST_WIDGET_H
#define PERSONS_LIST_WIDGET_H

#include <QWidget>
#include <QPushButton>

#include <rfl_tree.h>

#include <rgl_tree_widget.h>

class PersonsListWidget : public QWidget
{

    Q_OBJECT

    protected:

        FTree *familyTree;

        enum ColumnType
        {
            ColumnId = 0,
            ColumnSex,
            ColumnFirstName,
            ColumnMiddleName,
            ColumnLastName,
            ColumnMaidenName,
            ColumnBirthDate,
            ColumnBirthAddress,
            ColumnBirthCoordinates,
            ColumnDeathDate,
            ColumnDeathAddress,
            ColumnDeathCoordinates,
            NunberOfColumns
        };

        QPushButton *personAddButton;
        QPushButton *personEditButton;
        QPushButton *personRemoveButton;
        RTreeWidget *treeWidget;
        QAction *showTreeAction;
        QAction *mergePersonsAction;
        QAction *suggestPersonBirthAndDeathDatesAction;

    public:

        //! Constructor.
        explicit PersonsListWidget(FTree *familyTree, QWidget *parent = nullptr);

        //! Highlight persons.
        void highlightPersons(const QList<QUuid> &ids);

        //! Return list of selected persons.
        QList<QUuid> getSelectedIds() const;

    private:

        //! Populate list of persons.
        void populate();

        //! Return column name string.
        static QString getColumnName(ColumnType columnType);

        //! Set tree widget item values.
        static void setTreeWidgetItemValues(QTreeWidgetItem *item, const FPerson &person);

    protected slots:

        //! Show column.
        void showColumn(int column, bool checked);

        //! On item selection changed.
        void onItemSelectionChanged();

        //! On item double-clicked.
        void onItemDoubleClicked(QTreeWidgetItem *item, int);

        //! Person add button clicked.
        void onPersonAddButtonClicked();

        //! Person edit button clicked.
        void onPersonEditButtonClicked();

        //! Person remove button clicked.
        void onPersonRemoveButtonClicked();

        //! Add person.
        void addPerson(const FPerson &person);

        //! Change person.
        void changePerson(const FPerson &person);

        //! Remove person.
        void removePerson(const QUuid &personId);

        //! File loaded.
        void onFileLoaded(const QString &);

        //! Show family tree diagram of selected persons.
        void onShowDiagramDialog();

        //! Merge selected persons.
        void onMergePersonsAction();

        //! Find selected person birth date.
        void onSuggestPersonBirthAndDeathDatesAction();

        void personDiagramSelectionChanged(const QList<QUuid> &);


    signals:

        //! Selection changed.
        void selectionChanged(const QList<QUuid> &selectedIds);

};

#endif // PERSONS_LIST_WIDGET_H
