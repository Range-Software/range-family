#include <QAction>
#include <QDate>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QSignalBlocker>
#include <QStyle>
#include <QStyleHints>
#include <QToolButton>
#include <QTreeWidget>

#include "pane_header_widget.h"

//! Role of the first column holding the highlighted flag.
static constexpr int highlightRole = Qt::UserRole + 1;

PaneHeaderWidget::PaneHeaderWidget(const QString &title, QWidget *parent)
    : QWidget{parent}
    , title{title}
    , keepHighlighted{false}
    , onlyHighlighted{false}
    , filtered{false}
{
    this->mainLayout = new QHBoxLayout;
    this->mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(this->mainLayout);

    this->titleLabel = new QLabel(this->title);
    if (!this->title.isEmpty())
    {
        QFont titleFont = this->titleLabel->font();
        titleFont.setBold(true);
        this->titleLabel->setFont(titleFont);
    }
    this->mainLayout->addWidget(this->titleLabel);

    this->filterEdit = new QLineEdit;
    this->filterEdit->setPlaceholderText(tr("Filter"));
    this->filterEdit->setClearButtonEnabled(true);
    this->mainLayout->addWidget(this->filterEdit,1);

    QObject::connect(this->filterEdit,&QLineEdit::textChanged,this,&PaneHeaderWidget::filterChanged);
}

void PaneHeaderWidget::addButton(QAction *action)
{
    const int iconSize = this->style()->pixelMetric(QStyle::PM_SmallIconSize,nullptr,this);

    QToolButton *button = new QToolButton;
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setAutoRaise(true);
    button->setIconSize(QSize(iconSize,iconSize));
    this->mainLayout->addWidget(button);
}

void PaneHeaderWidget::setKeepHighlighted(bool keepHighlighted)
{
    this->keepHighlighted = keepHighlighted;
}

void PaneHeaderWidget::setOnlyHighlighted(bool onlyHighlighted)
{
    this->onlyHighlighted = onlyHighlighted;
}

void PaneHeaderWidget::filterItems(QTreeWidget *treeWidget)
{
    const int nTotal = treeWidget->topLevelItemCount();
    const QStringList words = this->filterEdit->text().split(' ',Qt::SkipEmptyParts);
    const bool restricted = !words.isEmpty() || this->onlyHighlighted;

    // Nothing to hide and nothing hidden.
    if (!restricted && !this->filtered)
    {
        this->setCount(nTotal,nTotal);
        return;
    }

    // Without highlighted items there would be nothing left to show.
    bool restrictToHighlighted = false;
    if (this->onlyHighlighted)
    {
        for (int i=0;i<nTotal && !restrictToHighlighted;i++)
        {
            restrictToHighlighted = PaneHeaderWidget::isItemHighlighted(treeWidget->topLevelItem(i));
        }
    }

    int nVisible = 0;
    bool selectionChanged = false;
    {
        // Change of selection is announced only once, after all items have been processed.
        // Listeners may highlight other items and so start another filtering.
        const QSignalBlocker treeWidgetBlocker(treeWidget);
        for (int i=0;i<nTotal;i++)
        {
            QTreeWidgetItem *item = treeWidget->topLevelItem(i);
            const bool highlighted = PaneHeaderWidget::isItemHighlighted(item);

            bool visible = PaneHeaderWidget::itemMatches(item,words);
            if (this->keepHighlighted && highlighted)
            {
                visible = true;
            }
            if (restrictToHighlighted && !highlighted)
            {
                visible = false;
            }

            if (!visible && item->isSelected())
            {
                item->setSelected(false);
                selectionChanged = true;
            }
            if (item->isHidden() == visible)
            {
                item->setHidden(!visible);
            }
            if (visible)
            {
                nVisible++;
            }
        }
    }
    this->filtered = restricted;

    this->setCount(nVisible,nTotal);

    if (selectionChanged)
    {
        emit treeWidget->itemSelectionChanged();
    }
}

void PaneHeaderWidget::clearFilter()
{
    this->filterEdit->clear();
}

void PaneHeaderWidget::setItemHighlighted(QTreeWidgetItem *item, bool highlighted)
{
    item->setData(0,highlightRole,highlighted);

    // Amber which leaves the text of both color schemes readable and differs from the selection.
    QBrush brush;
    if (highlighted)
    {
        const bool darkMode = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
        brush = QBrush(darkMode ? QColor(110,84,0) : QColor(255,229,138));
    }
    for (int column=0;column<item->columnCount();column++)
    {
        item->setBackground(column,brush);
    }
}

bool PaneHeaderWidget::isItemHighlighted(const QTreeWidgetItem *item)
{
    return item->data(0,highlightRole).toBool();
}

bool PaneHeaderWidget::itemMatches(const QTreeWidgetItem *item, const QStringList &words)
{
    if (words.isEmpty())
    {
        return true;
    }

    QStringList texts;
    for (int column=0;column<item->columnCount();column++)
    {
        // Only what is visible is searched, otherwise parts of hidden identifiers would match.
        if (item->treeWidget() && item->treeWidget()->isColumnHidden(column))
        {
            continue;
        }
        const QVariant value = item->data(column,Qt::DisplayRole);
        if (value.userType() == QMetaType::QDate)
        {
            // The same text as the one the view shows.
            texts.append(QLocale().toString(value.toDate(),QLocale::ShortFormat));
        }
        else
        {
            texts.append(value.toString());
        }
    }
    const QString text = texts.join('\n');

    for (const QString &word : words)
    {
        if (!text.contains(word,Qt::CaseInsensitive))
        {
            return false;
        }
    }
    return true;
}

void PaneHeaderWidget::setCount(int nVisible, int nTotal)
{
    QString count = QLocale().toString(nTotal);
    if (nVisible != nTotal)
    {
        count.prepend(QLocale().toString(nVisible) + " / ");
    }
    this->titleLabel->setText(this->title.isEmpty() ? count : this->title + " (" + count + ")");
}

void PaneHeaderWidget::focusFilter()
{
    this->filterEdit->setFocus(Qt::ShortcutFocusReason);
    this->filterEdit->selectAll();
}
