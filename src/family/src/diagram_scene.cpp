#include <QGraphicsPathItem>

#include "diagram_scene.h"
#include "graphics_node_item.h"
#include "graphics_edge_item.h"
#include "graphics_person_item.h"
#include "graphics_relation_item.h"

DiagramScene::DiagramScene(const FTree *familyTree, const QUuid &itemId, QObject *parent)
    : QGraphicsScene(parent)
    , familyTree(familyTree)
    , itemId(itemId)
{
    this->populate();

    QObject::connect(this->familyTree,&FTree::personAdded,this,&DiagramScene::onPersonAdded);
    QObject::connect(this->familyTree,&FTree::personChanged,this,&DiagramScene::onPersonChanged);
    QObject::connect(this->familyTree,&FTree::personRemoved,this,&DiagramScene::onPersonRemoved);
    QObject::connect(this->familyTree,&FTree::relationAdded,this,&DiagramScene::onRelationAdded);
    QObject::connect(this->familyTree,&FTree::relationChanged,this,&DiagramScene::onRelationChanged);
    QObject::connect(this->familyTree,&FTree::relationRemoved,this,&DiagramScene::onRelationRemoved);
    QObject::connect(this->familyTree,&FTree::fileLoaded,this,&DiagramScene::onFileLoaded);

    QObject::connect(this,&DiagramScene::selectionChanged,this,&DiagramScene::onSelectionChanged);
}

void DiagramScene::setItemId(const QUuid &itemId)
{
    if (this->itemId == itemId)
    {
        return;
    }
    this->itemId = itemId;

    this->populate();

    emit this->itemIdChanged(this->itemId);
}

const FTreeModel &DiagramScene::getTreeModel() const
{
    return this->treeModel;
}

void DiagramScene::populate()
{
    this->clear();

    if (!this->familyTree->containsPerson(this->itemId) && !this->familyTree->containsRelation(this->itemId))
    {
        return;
    }

    this->treeModel.build(this->familyTree,this->itemId);
    qreal padding = 10.0;
    qreal slotWidth = 250.0;
    qreal slotHeight = 100.0;
    QRectF sceneSize(0.0,0.0,0.0,0.0);

    QHash<const FTreeModelItem*,const GraphicsNodeItem*> itemsMap;

    for (uint i=0;i<this->treeModel.getNVRanks();i++)
    {
        foreach (const FTreeModelItem *modelItem, this->treeModel.getItems(i))
        {
            if (!modelItem->getVisible())
            {
                continue;
            }
            QRectF slot(slotWidth * (modelItem->getHRank() - this->treeModel.getMinHRank()),
                        slotHeight * i,
                        slotWidth,
                        slotHeight);

            if (modelItem->getType() == FTreeModelItem::Type::Person)
            {
                FPerson person = this->familyTree->findPerson(modelItem->getId());
                GraphicsPersonItem *personItem = new GraphicsPersonItem(person,slot);
                personItem->setHighlighted(person.getId() == this->itemId);

                itemsMap[modelItem] = personItem;
                this->addItem(personItem);
            }
            else if (modelItem->getType() == FTreeModelItem::Type::Relation)
            {
                FRelation relation = this->familyTree->findRelation(modelItem->getId());
                GraphicsRelationItem *relationItem = new GraphicsRelationItem(relation,slot);
                relationItem->setHighlighted(relation.getId() == this->itemId);

                itemsMap[modelItem] = relationItem;
                this->addItem(relationItem);
            }
            else
            {
                continue;
            }

            sceneSize.setX(std::min(sceneSize.x(),slot.x() - padding));
            sceneSize.setY(std::min(sceneSize.y(),slot.y() - padding));

            sceneSize.setWidth(std::max(sceneSize.width(),slot.x() + slot.width() + 2*padding));
            sceneSize.setHeight(std::max(sceneSize.height(),slot.y() + slot.height() + 2*padding));
        }
    }

    foreach (const FTreeModelItem *item, this->treeModel.getMap())
    {
        if (!item->getVisible())
        {
            continue;
        }
        if (item->getType() == FTreeModelItem::Type::Relation)
        {
            if (!itemsMap.contains(item))
            {
                continue;
            }
            const GraphicsRelationItem *relationGraphicItem = qgraphicsitem_cast<const GraphicsRelationItem*>(itemsMap[item]);

            foreach (const FTreeModelItem *personItem,item->getPartners())
            {
                if (!itemsMap.contains(personItem))
                {
                    continue;
                }
                const GraphicsNodeItem *personGraphicItem = qgraphicsitem_cast<const GraphicsNodeItem*>(itemsMap[personItem]);
                GraphicsEdgeItem *edge = new GraphicsEdgeItem(personGraphicItem,relationGraphicItem);
                edge->setZValue(edge->zValue() - 0.1f);
                this->addItem(edge);
            }
            foreach (const FTreeModelItem *personItem,item->getChildren())
            {
                if (!itemsMap.contains(personItem))
                {
                    continue;
                }
                const GraphicsNodeItem *personGraphicItem = qgraphicsitem_cast<const GraphicsNodeItem*>(itemsMap[personItem]);
                GraphicsEdgeItem *edge = new GraphicsEdgeItem(relationGraphicItem,personGraphicItem);
                edge->setZValue(edge->zValue() - 0.1f);
                this->addItem(edge);
            }
        }
    }

    this->setSceneRect(sceneSize);
    emit this->populated();
}

void DiagramScene::onPersonAdded(const FPerson &)
{
    this->populate();
}

void DiagramScene::onPersonChanged(const FPerson &)
{
    this->populate();
}

void DiagramScene::onPersonRemoved(QUuid)
{
    this->populate();
}

void DiagramScene::onRelationAdded(const FRelation &)
{
    this->populate();
}

void DiagramScene::onRelationChanged(const FRelation &)
{
    this->populate();
}

void DiagramScene::onRelationRemoved(QUuid)
{
    this->populate();
}

void DiagramScene::onFileLoaded(const QString &)
{
    this->populate();
}

void DiagramScene::onSelectionChanged()
{
    QList<QUuid> selectedPersonsIds;
    QList<QUuid> selectedRelationsIds;

    foreach (QGraphicsItem *item, this->selectedItems())
    {
        if (const GraphicsPersonItem *personItem = qgraphicsitem_cast<const GraphicsPersonItem*>(item))
        {
            selectedPersonsIds.append(personItem->getPersonId());
        }
        if (const GraphicsRelationItem *relationItem = qgraphicsitem_cast<const GraphicsRelationItem*>(item))
        {
            selectedRelationsIds.append(relationItem->getRelationId());
        }
    }

    emit this->personSelectionChanged(selectedPersonsIds);
    emit this->relationSelectionChanged(selectedRelationsIds);
}
