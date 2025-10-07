#ifndef DIAGRAM_WIDGET_H
#define DIAGRAM_WIDGET_H

#include <QUuid>
#include <QWidget>

#include "diagram_scene.h"
#include "diagram_view.h"

class DiagramWidget : public QWidget
{

    Q_OBJECT

    protected:

        const FTree *familyTree;

        QUuid itemId;

        DiagramView *view;
        DiagramScene *scene;

    public:

        //! Constructor.
        explicit DiagramWidget(FTree *familyTree, const QUuid &itemId, QWidget *parent = nullptr);

        //! Set new item ID.
        void setItemId(const QUuid &itemId);

    protected slots:

        void onExportTriggered();

        void onItemIdActivated(const QUuid &itemId);

        void onItemIdChanged(const QUuid &itemId);

        void onScenePopulated();

        void onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

        void onScaleFactorChanged(double factor);

    signals:

        void itemIdChanged(const QUuid &itemId);

        void personSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void relationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

};

#endif // DIAGRAM_WIDGET_H
