#include <QGraphicsItem>
#include <QMouseEvent>
#include <QLocale>

#include <rbl_logger.h>
#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_person.h>
#include <rfl_relation.h>
#include <rfl_tool_action.h>

#include <rgl_message_box.h>

#include "diagram_view.h"
#include "person_add_dialog.h"
#include "person_edit_dialog.h"
#include "relation_add_dialog.h"
#include "relation_edit_dialog.h"

DiagramView::DiagramView(FTree *familyTree, QWidget *parent)
    : QGraphicsView{parent}
    , familyTree{familyTree}
    , savedScaleValue{1.0}
{
    R_LOG_TRACE_IN;
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setMouseTracking(true);
    this->setDragMode(QGraphicsView::ScrollHandDrag);

    this->setRenderHint(QPainter::Antialiasing, true);
    // this->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    // this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    // this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->newPartnerRelation = this->addAction(DiagramView::tr("New relation (partner)"));
    QObject::connect(this->newPartnerRelation,&QAction::triggered,this,&DiagramView::onNewPartnerRelation);

    this->newChildRelation = this->addAction(DiagramView::tr("New relation (child)"));
    QObject::connect(this->newChildRelation,&QAction::triggered,this,&DiagramView::onNewChildRelation);

    this->newPartner = this->addAction(DiagramView::tr("New partner"));
    QObject::connect(this->newPartner,&QAction::triggered,this,&DiagramView::onNewPartner);

    this->newChild = this->addAction(DiagramView::tr("New child"));
    QObject::connect(this->newChild,&QAction::triggered,this,&DiagramView::onNewChild);

    this->mergePersons = this->addAction(DiagramView::tr("Merge persons"));
    QObject::connect(this->mergePersons,&QAction::triggered,this,&DiagramView::onMergePersons);

    this->editItem = this->addAction(DiagramView::tr("Edit"));
    QObject::connect(this->editItem,&QAction::triggered,this,&DiagramView::onEditItem);

    this->removeItem = this->addAction(DiagramView::tr("Remove"));
    QObject::connect(this->removeItem,&QAction::triggered,this,&DiagramView::onRemoveItem);
    R_LOG_TRACE_OUT;
}

void DiagramView::centerOnItem(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    foreach (const QGraphicsItem *item, this->items())
    {
        if (const GraphicsPersonItem *personItem = qgraphicsitem_cast<const GraphicsPersonItem*>(item))
        {
            if (personItem->getPersonId() == itemId)
            {
                this->centerOn(personItem->getCenterPosition());
            }
        }
        if (const GraphicsRelationItem *relationItem = qgraphicsitem_cast<const GraphicsRelationItem*>(item))
        {
            if (relationItem->getRelationId() == itemId)
            {
                this->centerOn(relationItem->getCenterPosition());
            }
        }
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::zoom(double factor, ViewportAnchor anchor)
{
    R_LOG_TRACE_IN;
    this->savedScaleValue *= factor;
    const QGraphicsView::ViewportAnchor anchorSave = this->transformationAnchor();
    this->setTransformationAnchor(anchor);
    this->scale(factor,factor);
    this->setTransformationAnchor(anchorSave);
    emit this->scaleChanged(this->savedScaleValue);
    R_LOG_TRACE_OUT;
}

void DiagramView::mousePressEvent(QMouseEvent *event)
{
    R_LOG_TRACE_IN;

    this->processPickedItems(this->items(event->pos()));
    this->processSelectedItems(this->items());

    if (event->buttons() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)
    {
        foreach (const GraphicsPersonItem *personItem, this->pickedPersons)
        {
            emit this->itemIdActivated(personItem->getPersonId());
            R_LOG_TRACE_OUT;
            return;
        }
        foreach (const GraphicsRelationItem *relationItem, this->pickedRelations)
        {
            emit this->itemIdActivated(relationItem->getRelationId());
            R_LOG_TRACE_OUT;
            return;
        }
    }

    this->newPartnerRelation->setDisabled(this->pickedPersons.isEmpty());
    this->newChildRelation->setDisabled(this->pickedPersons.isEmpty());
    this->newPartner->setDisabled(this->pickedRelations.isEmpty());
    this->newChild->setDisabled(this->pickedRelations.isEmpty());
    this->mergePersons->setDisabled(this->selectedPersons.size() < 2);
    this->editItem->setDisabled(this->pickedPersons.isEmpty() && this->pickedRelations.isEmpty());
    this->removeItem->setDisabled(this->pickedPersons.isEmpty() && this->pickedRelations.isEmpty());

    this->QGraphicsView::mousePressEvent(event);

    R_LOG_TRACE_OUT;
}

void DiagramView::mouseReleaseEvent(QMouseEvent *event)
{
    R_LOG_TRACE_IN;
    this->QGraphicsView::mouseReleaseEvent(event);
    R_LOG_TRACE_OUT;
}

void DiagramView::mouseMoveEvent(QMouseEvent *event)
{
    R_LOG_TRACE_IN;
    this->QGraphicsView::mouseMoveEvent(event);
    QGraphicsView::DragMode dragMode = QGraphicsView::ScrollHandDrag;
    QList<QGraphicsItem*> itemsUnderMouse = this->items(event->pos());
    for (QGraphicsItem *item : std::as_const(itemsUnderMouse))
    {
        if (qgraphicsitem_cast<const GraphicsPersonItem*>(item) || qgraphicsitem_cast<const GraphicsRelationItem*>(item))
        {
            dragMode = QGraphicsView::NoDrag;
        }
    }
    if (this->dragMode() != dragMode)
    {
        this->setDragMode(dragMode);
    }
    if (dragMode == QGraphicsView::NoDrag)
    {
        this->scene()->update();
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::mouseDoubleClickEvent(QMouseEvent *event)
{
    R_LOG_TRACE_IN;

    this->QGraphicsView::mouseDoubleClickEvent(event);

    this->onEditItem();

    R_LOG_TRACE_OUT;
}

void DiagramView::wheelEvent(QWheelEvent *event)
{
    R_LOG_TRACE_IN;

    if (event->modifiers() & Qt::ControlModifier)
    {
        this->zoom(qPow(1.001, event->angleDelta().y()),QGraphicsView::AnchorUnderMouse);
    }
    else
    {
        this->QGraphicsView::wheelEvent(event);
    }

    R_LOG_TRACE_OUT;
}

void DiagramView::processPickedItems(const QList<QGraphicsItem *> pickedItems)
{
    R_LOG_TRACE_IN;
    this->pickedPersons.clear();
    this->pickedRelations.clear();

    foreach (const QGraphicsItem *item, pickedItems)
    {
        if (const GraphicsPersonItem *personItem = qgraphicsitem_cast<const GraphicsPersonItem*>(item))
        {
            this->pickedPersons.append(personItem);
        }
        if (const GraphicsRelationItem *relationItem = qgraphicsitem_cast<const GraphicsRelationItem*>(item))
        {
            this->pickedRelations.append(relationItem);
        }
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::processSelectedItems(const QList<QGraphicsItem *> selectedItems)
{
    R_LOG_TRACE_IN;
    this->selectedPersons.clear();
    this->selectedRelations.clear();

    foreach (const QGraphicsItem *item, selectedItems)
    {
        if (item->isSelected())
        {
            if (const GraphicsPersonItem *personItem = qgraphicsitem_cast<const GraphicsPersonItem*>(item))
            {
                this->selectedPersons.append(personItem);
            }
            if (const GraphicsRelationItem *relationItem = qgraphicsitem_cast<const GraphicsRelationItem*>(item))
            {
                this->selectedRelations.append(relationItem);
            }
        }
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::onNewPartnerRelation()
{
    R_LOG_TRACE_IN;
    QList<QUuid> partners;
    foreach (const GraphicsPersonItem *personItem, this->pickedPersons)
    {
        partners.append(personItem->getPersonId());
    }

    FRelation newRelation;
    newRelation.setPartners(partners);

    RelationAddDialog relationAddDialog(this->familyTree,newRelation,this);
    relationAddDialog.exec();
    R_LOG_TRACE_OUT;
}

void DiagramView::onNewChildRelation()
{
    R_LOG_TRACE_IN;
    QList<QUuid> children;
    foreach (const GraphicsPersonItem *personItem, this->pickedPersons)
    {
        children.append(personItem->getPersonId());
    }

    FRelation newRelation;
    newRelation.setChildren(children);

    RelationAddDialog relationAddDialog(this->familyTree,newRelation,this);
    relationAddDialog.exec();
    R_LOG_TRACE_OUT;
}

void DiagramView::onNewPartner()
{
    R_LOG_TRACE_IN;
    foreach (const GraphicsRelationItem *relationItem, this->pickedRelations)
    {
        PersonAddDialog personAddDialog(this->familyTree,FPerson(),this);
        int retVal = personAddDialog.exec();

        if (retVal == QDialog::Accepted)
        {
            FRelation relation = this->familyTree->findRelation(relationItem->getRelationId());
            QList<QUuid> partners = relation.getPartners();
            partners.append(personAddDialog.getNewPersonId());
            relation.setPartners(partners);

            RToolInput toolInput;
            toolInput.addAction(FToolAction::modifyRelation(this->familyTree,relation));
            RToolTask *toolTask = new RToolTask(toolInput);
            RJobManager::getInstance().submit(toolTask);
        }
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::onNewChild()
{
    R_LOG_TRACE_IN;
    foreach (const GraphicsRelationItem *relationItem, this->pickedRelations)
    {
        PersonAddDialog personAddDialog(this->familyTree,FPerson(),this);
        int retVal = personAddDialog.exec();

        if (retVal == QDialog::Accepted)
        {
            FRelation relation = this->familyTree->findRelation(relationItem->getRelationId());
            QList<QUuid> children = relation.getChildren();
            children.append(personAddDialog.getNewPersonId());
            relation.setChildren(children);

            RToolInput toolInput;
            toolInput.addAction(FToolAction::modifyRelation(this->familyTree,relation));
            RToolTask *toolTask = new RToolTask(toolInput);
            RJobManager::getInstance().submit(toolTask);
        }
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::onMergePersons()
{
    R_LOG_TRACE_IN;
    QList<QUuid> persons;
    foreach (const GraphicsPersonItem *personItem, this->selectedPersons)
    {
        persons.append(personItem->getPersonId());
    }

    QString question = tr("Are you sure you want to merge selected persons?");

    question += "<ul>";
    foreach (QUuid id, persons)
    {
        question += "<li>" + this->familyTree->findPerson(id).getName().toString() + "</li>";
    }
    question += "</ul>";

    if (RMessageBox::question(this,tr("Merge persons"),question) == RMessageBox::Yes)
    {
        this->familyTree->mergePersons(persons);
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::onEditItem()
{
    R_LOG_TRACE_IN;
    foreach (const GraphicsPersonItem *personItem, this->pickedPersons)
    {
        PersonEditDialog personEditDialog(this->familyTree,this->familyTree->findPerson(personItem->getPersonId()),this);
        personEditDialog.exec();
    }
    foreach (const GraphicsRelationItem *relationItem, this->pickedRelations)
    {
        RelationEditDialog relationEditDialog(this->familyTree,this->familyTree->findRelation(relationItem->getRelationId()),this);
        relationEditDialog.exec();
    }
    R_LOG_TRACE_OUT;
}

void DiagramView::onRemoveItem()
{
    R_LOG_TRACE_IN;
    foreach (const GraphicsPersonItem *personItem, this->pickedPersons)
    {
        FPerson person = this->familyTree->findPerson(personItem->getPersonId());

        QString question  = tr("Are you sure you want to remove selected person?")
                          + "<p>"
                          + "<b>" + person.getName().toString() + "</b>"
                          + " (" + QLocale().toString(person.getBirth().getDate().toDate()) + " - " + QLocale().toString(person.getDeath().getDate().toDate()) + ")"
                          + "</p>";

        int response = RMessageBox::question(this,tr("Remove person"),question);
        if (response == RMessageBox::Yes)
        {

            RToolInput toolInput;
            toolInput.addAction(FToolAction::removePerson(this->familyTree,personItem->getPersonId()));
            RToolTask *toolTask = new RToolTask(toolInput);
            RJobManager::getInstance().submit(toolTask);
        }
    }
    foreach (const GraphicsRelationItem *relationItem, this->pickedRelations)
    {
        FRelation relation = this->familyTree->findRelation(relationItem->getRelationId());

        QString question  = tr("Are you sure you want to remove selected relation?")
                          + "<p>"
                          + "<b>" + tr("Children") + ": " + QLocale().toString(relation.getChildren().size()) + "</b>"
                          + " (" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + ")"
                          + "<p>"
                          + "<i>" + tr("No person will be removed.") + "</i>"
                          + "</p>";

        int response = RMessageBox::question(this,tr("Remove person"),question);
        if (response == RMessageBox::Yes)
        {

            RToolInput toolInput;
            toolInput.addAction(FToolAction::removeRelation(this->familyTree,relationItem->getRelationId()));
            RToolTask *toolTask = new RToolTask(toolInput);
            RJobManager::getInstance().submit(toolTask);
        }
    }
    R_LOG_TRACE_OUT;
}
