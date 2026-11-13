#ifndef DATE_TREE_WIDGET_ITEM_H
#define DATE_TREE_WIDGET_ITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <rfl_date.h>

//! Tree widget item which sorts date columns chronologically instead of by their text.
class DateTreeWidgetItem : public QTreeWidgetItem
{

    public:

        static const int SortRole = Qt::UserRole + 1;

        using QTreeWidgetItem::QTreeWidgetItem;

        //! Show date in given column.
        static void setDate(QTreeWidgetItem *item, int column, const FDate &date)
        {
            item->setText(column,date.toLocaleString(QLocale::ShortFormat));
            item->setData(column,DateTreeWidgetItem::SortRole,date.toDate());
        }

        bool operator <(const QTreeWidgetItem &other) const override
        {
            int column = this->treeWidget() ? this->treeWidget()->sortColumn() : 0;
            QVariant thisDate = this->data(column,DateTreeWidgetItem::SortRole);
            QVariant otherDate = other.data(column,DateTreeWidgetItem::SortRole);
            if (thisDate.isValid() && otherDate.isValid())
            {
                return thisDate.toDate() < otherDate.toDate();
            }
            return QTreeWidgetItem::operator <(other);
        }

};

#endif // DATE_TREE_WIDGET_ITEM_H
