#ifndef PERSONS_LIST_WIDGET_H
#define PERSONS_LIST_WIDGET_H

#include <QWidget>

#include <rfl_tree.h>

#include <rgl_tree_widget.h>

#include "pane_header_widget.h"
#include "scroll_bar_with_markers.h"

class QCheckBox;

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
            NumberOfColumns
        };

        //! Header with title, filter and buttons.
        PaneHeaderWidget *header;
        //! Check box keeping highlighted persons visible regardless of the filter.
        QCheckBox *keepHighlightedCheckBox;
        //! ID of the active person.
        QUuid activePersonId;
        RTreeWidget *treeWidget;
        ScrollBarWithMarkers *scrollBarWithMarkers;

    public:

        //! Constructor.
        explicit PersonsListWidget(FTree *familyTree, QWidget *parent = nullptr);

        //! Highlight persons.
        void highlightPersons(const QList<QUuid> &ids);

        //! Select given person only and scroll to it. Filter is removed if it hides the person.
        void selectPerson(const QUuid &id);

        //! Return list of selected persons.
        QList<QUuid> getSelectedIds() const;

    private:

        //! Populate list of persons.
        void populate();

        //! Place markers of highlighted and active persons on the scroll bar.
        void updateMarkers();

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

        //! Add person.
        void addPerson(const FPerson &person);

        //! Change person.
        void changePerson(const FPerson &person);

        //! Remove person.
        void removePerson(const QUuid &personId);

        //! File loaded.
        void onFileLoaded(const QString &);

        //! Show only persons matching the filter.
        void applyFilter();

        //! Check box keeping highlighted persons visible has been toggled.
        void onKeepHighlightedToggled(bool checked);

        //! Application style or color scheme has been applied.
        void onStyleApplied();

        void personDiagramSelectionChanged(const QList<QUuid> &);

    public slots:

        //! Add new person.
        void addNewPerson();

        //! Edit selected persons.
        void editSelectedPersons();

        //! Remove selected persons.
        void removeSelectedPersons();

        //! Show family tree diagram of selected persons.
        void showSelectedPersonsDiagram();

        //! Set selected person as active.
        void setSelectedPersonActive();

        //! Merge selected persons.
        void mergeSelectedPersons();

        //! Suggest birth and death dates of selected persons.
        void suggestSelectedPersonsDates();

        //! Enable actions and buttons which can be used with the current selection.
        void updateActionsAvailability();

        //! Active person ID changed.
        void onActivePersonIdChanged(const QUuid &id);


    signals:

        //! Selection changed.
        void selectionChanged(const QList<QUuid> &selectedIds);

};

#endif // PERSONS_LIST_WIDGET_H
