#ifndef PANE_HEADER_WIDGET_H
#define PANE_HEADER_WIDGET_H

#include <QWidget>

class QAction;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;

//! Header of a list pane with its title, number of items, filter and buttons of actions.
//! It also decides which items of the list are visible.
class PaneHeaderWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! Title of the pane, empty if the title is shown elsewhere.
        QString title;
        //! Label with title and number of items.
        QLabel *titleLabel;
        //! Filter edit.
        QLineEdit *filterEdit;
        //! Layout of the header.
        QHBoxLayout *mainLayout;
        //! Highlighted items are visible even if they do not match the filter.
        bool keepHighlighted;
        //! Only highlighted items are visible, as long as there are some.
        bool onlyHighlighted;
        //! Some items could have been hidden by the last filtering.
        bool filtered;

    public:

        //! Constructor. Empty title leaves only the number of items in the header.
        explicit PaneHeaderWidget(const QString &title, QWidget *parent = nullptr);

        //! Add button triggering given action.
        void addButton(QAction *action);

        //! Set whether highlighted items stay visible even if they do not match the filter.
        void setKeepHighlighted(bool keepHighlighted);

        //! Set whether only highlighted items are visible, as long as there are some.
        void setOnlyHighlighted(bool onlyHighlighted);

        //! Show only items of given tree widget which pass the filter and update number of items in the header.
        //! Items which get hidden are deselected, so no action works with an item which is not visible.
        void filterItems(QTreeWidget *treeWidget);

        //! Remove filter text.
        void clearFilter();

        //! Mark given item as highlighted and color it for the current color scheme.
        static void setItemHighlighted(QTreeWidgetItem *item, bool highlighted);

        //! Return true if given item is marked as highlighted.
        static bool isItemHighlighted(const QTreeWidgetItem *item);

    private:

        //! Return true if visible columns of given item contain all given words.
        static bool itemMatches(const QTreeWidgetItem *item, const QStringList &words);

        //! Show number of items in the header.
        void setCount(int nVisible, int nTotal);

    public slots:

        //! Move keyboard focus to the filter.
        void focusFilter();

    signals:

        //! Filter text has changed.
        void filterChanged(const QString &text);

};

#endif // PANE_HEADER_WIDGET_H
