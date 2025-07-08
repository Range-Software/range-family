#ifndef GRAPHICS_RELATION_ITEM_H
#define GRAPHICS_RELATION_ITEM_H

#include <rfl_relation.h>

#include "graphics_node_item.h"

class GraphicsRelationItem : public GraphicsNodeItem
{

    protected:

        FRelation relation;

    public:

        explicit GraphicsRelationItem(const FRelation &relation, const QRectF &slot, GraphicsNodeItem *parent = nullptr);

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

        const QUuid &getRelationId() const;

        enum { Type = UserType + 2 };

        int type() const override
        {
            // Enable the use of qgraphicsitem_cast with this item.
            return Type;
        }

};

#endif // GRAPHICS_RELATION_ITEM_H
