#ifndef DIAGRAM_DIALOG_H
#define DIAGRAM_DIALOG_H

#include <QDialog>
#include <QWidget>

#include <rfl_tree.h>

#include "diagram_widget.h"

class DiagramDialog : public QDialog
{

    Q_OBJECT

    protected:

        FTree *familyTree;
        QUuid itemId;

        DiagramWidget *diagram;

    public:

        //! Constructor.
        explicit DiagramDialog(FTree *familyTree, const QUuid &itemId, QWidget *parent = nullptr);

        //! Set new item ID.
        void setItemId(const QUuid &itemId);


    private:

        //! Set window title.
        void setWindowTitle(const QUuid &itemId);

    protected slots:

        void onItemIdChanged(const QUuid &itemId);

        void opPersonRemoved(const QUuid personId);

        void opRelationRemoved(const QUuid relationId);

        void onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

    signals:

        void itemIdChanged(const QUuid &itemId);

        void personSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void relationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

};

#endif // DIAGRAM_DIALOG_H
