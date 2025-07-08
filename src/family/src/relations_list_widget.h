#ifndef RELATIONS_LIST_WIDGET_H
#define RELATIONS_LIST_WIDGET_H

#include <QWidget>
#include <QPushButton>

#include <rfl_tree.h>

#include <rgl_tree_widget.h>

class RelationsListWidget : public QWidget
{

    Q_OBJECT

    protected:

        FTree *familyTree;

        enum ColumnType
        {
            ColumnId = 0,
            ColumnPartners,
            ColumnChildren,
            ColumnBeginDate,
            ColumnBeginAddress,
            ColumnBeginCoordinates,
            ColumnEndDate,
            ColumnEndAddress,
            ColumnEndCoordinates,
            NunberOfColumns
        };

        QPushButton *relationAddButton;
        QPushButton *relationEditButton;
        QPushButton *relationRemoveButton;
        RTreeWidget *treeWidget;
        QAction *showTreeAction;

    public:

        //! Constructor.
        explicit RelationsListWidget(FTree *familyTree, QWidget *parent = nullptr);

        //! Highlight relations.
        void highlightRelations(const QList<QUuid> &ids);

        //! Return list of selected persons.
        QList<QUuid> getSelectedIds() const;

    private:

        //! Populate list of persons.
        void populate();

        //! Return column name string.
        static QString getColumnName(ColumnType columnType);

        //! Set tree widget item values.
        void setTreeWidgetItemValues(QTreeWidgetItem *item, const FRelation &relation) const;

        //! Return partners list as string.
        QString getPartnersListString(const FRelation &relation) const;

        //! Return partners list as string.
        QString getChildrenListString(const FRelation &relation) const;

    protected slots:

        //! Show column.
        void showColumn(int column, bool checked);

        //! On item selection changed.
        void onItemSelectionChanged();

        //! On item double-clicked.
        void onItemDoubleClicked(QTreeWidgetItem *item, int);

        //! Relation add button clicked.
        void onRelationAddButtonClicked();

        //! Relation edit button clicked.
        void onRelationEditButtonClicked();

        //! Relation remove button clicked.
        void onRelationRemoveButtonClicked();

        //! Add relation.
        void addRelation(const FRelation &relation);

        //! Change relation.
        void changeRelation(const FRelation &relation);

        //! Remove relation.
        void removeRelation(const QUuid &relationId);

        //! Change person.
        void changePerson(const FPerson &person);

        //! File loaded.
        void onFileLoaded(const QString &);

        //! Show family tree diagram of selected relation.
        void onShowDiagramDialog();

        void relationDiagramSelectionChanged(const QList<QUuid> &);

    signals:

        //! Selection changed.
        void selectionChanged(const QList<QUuid> &selectedIds);

};

#endif // RELATIONS_LIST_WIDGET_H
