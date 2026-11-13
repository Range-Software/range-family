#ifndef RELATIONS_LIST_WIDGET_H
#define RELATIONS_LIST_WIDGET_H

#include <QWidget>

#include <rfl_tree.h>

#include <rgl_tree_widget.h>

#include "pane_header_widget.h"
#include "scroll_bar_with_markers.h"

class QCheckBox;

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
            NumberOfColumns
        };

        //! Header with title, filter and buttons.
        PaneHeaderWidget *header;
        //! Check box keeping highlighted relations visible regardless of the filter.
        QCheckBox *keepHighlightedCheckBox;
        //! Check box showing only highlighted relations.
        QCheckBox *onlyHighlightedCheckBox;
        RTreeWidget *treeWidget;
        ScrollBarWithMarkers *scrollBarWithMarkers;

    public:

        //! Constructor.
        explicit RelationsListWidget(FTree *familyTree, QWidget *parent = nullptr);

        //! Highlight relations.
        void highlightRelations(const QList<QUuid> &ids);

        //! Select given relation only and scroll to it. Filter is removed if it hides the relation.
        void selectRelation(const QUuid &id);

        //! Return list of selected relations.
        QList<QUuid> getSelectedIds() const;

    private:

        //! Populate list of relations.
        void populate();

        //! Place markers of highlighted relations on the scroll bar.
        void updateMarkers();

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

        //! Show only relations matching the filter.
        void applyFilter();

        //! Check box keeping highlighted relations visible has been toggled.
        void onKeepHighlightedToggled(bool checked);

        //! Check box showing only highlighted relations has been toggled.
        void onOnlyHighlightedToggled(bool checked);

        //! Application style or color scheme has been applied.
        void onStyleApplied();

        void relationDiagramSelectionChanged(const QList<QUuid> &);

    public slots:

        //! Add new relation.
        void addNewRelation();

        //! Edit selected relations.
        void editSelectedRelations();

        //! Remove selected relations.
        void removeSelectedRelations();

        //! Show family tree diagram of selected relations.
        void showSelectedRelationsDiagram();

        //! Enable actions and buttons which can be used with the current selection.
        void updateActionsAvailability();

    signals:

        //! Selection changed.
        void selectionChanged(const QList<QUuid> &selectedIds);

};

#endif // RELATIONS_LIST_WIDGET_H
