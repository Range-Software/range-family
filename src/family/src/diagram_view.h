#ifndef DIAGRAM_VIEW_H
#define DIAGRAM_VIEW_H

#include <QUuid>
#include <QGraphicsView>

#include <rfl_tree_model.h>

#include "graphics_person_item.h"
#include "graphics_relation_item.h"

class DiagramView : public QGraphicsView
{

    Q_OBJECT

    protected:

        FTree *familyTree;

        //! Saved scale value.
        double savedScaleValue;

        QList<const GraphicsPersonItem*> pickedPersons;
        QList<const GraphicsRelationItem*> pickedRelations;

        QList<const GraphicsPersonItem*> selectedPersons;
        QList<const GraphicsRelationItem*> selectedRelations;

        QAction *newPartnerRelation;
        QAction *newChildRelation;
        QAction *newPartner;
        QAction *newChild;
        QAction *mergePersons;
        QAction *editItem;
        QAction *removeItem;

    public:

        //! Constructor.
        explicit DiagramView(FTree *familyTree, QWidget *parent = nullptr);

        //! Center view on item.
        void centerOnItem(const QUuid &itemId);

        //! Zoom view.
        void zoom(double factor, QGraphicsView::ViewportAnchor anchor);

    protected:

        void mousePressEvent(QMouseEvent *event) override;

        void mouseReleaseEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        void mouseDoubleClickEvent(QMouseEvent *event) override;

        void wheelEvent(QWheelEvent *event) override;

    private:

        void processPickedItems(const QList<QGraphicsItem*> pickedItems);

        void processSelectedItems(const QList<QGraphicsItem*> selectedItems);

    protected slots:

        void onNewPartnerRelation();

        void onNewChildRelation();

        void onNewPartner();

        void onNewChild();

        void onMergePersons();

        void onEditItem();

        void onRemoveItem();

    signals:

        void itemIdActivated(const QUuid &itemId);

        void personSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void relationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

        void personHover(const QUuid &personId);

        void relationHover(const QUuid &relationId);

        void scaleChanged(double scale);

};

#endif // DIAGRAM_VIEW_H
