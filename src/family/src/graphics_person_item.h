#ifndef GRAPHICS_PERSON_ITEM_H
#define GRAPHICS_PERSON_ITEM_H

#include <rfl_person.h>

#include "graphics_node_item.h"

class GraphicsPersonItem : public GraphicsNodeItem
{

    public:

        FPerson person;

    public:

        explicit GraphicsPersonItem(const FPerson &person, const QRectF &slot, GraphicsNodeItem *parent = nullptr);

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

        const QUuid &getPersonId() const;

        enum { Type = UserType + 1 };

        int type() const override
        {
            // Enable the use of qgraphicsitem_cast with this item.
            return Type;
        }

};

#endif // GRAPHICS_PERSON_ITEM_H
