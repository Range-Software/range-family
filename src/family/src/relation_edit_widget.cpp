#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>

#include "relation_edit_widget.h"

#define _FR_TR(qstr) qApp->translate("FRelation",qstr.toUtf8().constData())

RelationEditWidget::RelationEditWidget(const FTree *familyTree, const FRelation &relation, QWidget *parent)
    : QWidget(parent)
    , familyTree(familyTree)
    , relation(relation)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QGroupBox *partnersGroupBox = new QGroupBox(_FR_TR(FRelation::ElementDesc::partners));
    mainLayout->addWidget(partnersGroupBox);

    QVBoxLayout *partnersLayout = new QVBoxLayout;
    partnersGroupBox->setLayout(partnersLayout);

    this->partnersEdit = new PersonsListEditWidget(this->familyTree,this->relation.getPartners(),this->relation.getChildren());
    partnersLayout->addWidget(this->partnersEdit);

    QGroupBox *childrenGroupBox = new QGroupBox(_FR_TR(FRelation::ElementDesc::children));
    mainLayout->addWidget(childrenGroupBox);

    QVBoxLayout *childrenLayout = new QVBoxLayout;
    childrenGroupBox->setLayout(childrenLayout);

    this->childrenEdit = new PersonsListEditWidget(this->familyTree,this->relation.getChildren(),this->relation.getPartners());
    childrenLayout->addWidget(this->childrenEdit);

    QGroupBox *beginGroupBox = new QGroupBox(_FR_TR(FRelation::ElementDesc::begin));
    mainLayout->addWidget(beginGroupBox);

    QVBoxLayout *beginLayout = new QVBoxLayout;
    beginGroupBox->setLayout(beginLayout);

    this->beginEdit = new EventEditWidget(this->relation.getBegin());
    beginLayout->addWidget(this->beginEdit);

    QGroupBox *endGroupBox = new QGroupBox(_FR_TR(FRelation::ElementDesc::end));
    mainLayout->addWidget(endGroupBox);

    QVBoxLayout *endLayout = new QVBoxLayout;
    endGroupBox->setLayout(endLayout);

    this->endEdit = new EventEditWidget(this->relation.getEnd());
    endLayout->addWidget(this->endEdit);

    QObject::connect(this->partnersEdit,&PersonsListEditWidget::changed,this,&RelationEditWidget::onPartnersListChanged);
    QObject::connect(this->childrenEdit,&PersonsListEditWidget::changed,this,&RelationEditWidget::onChildrenListChanged);
    QObject::connect(this->beginEdit,&EventEditWidget::changed,this,&RelationEditWidget::onBeginEventChanged);
    QObject::connect(this->endEdit,&EventEditWidget::changed,this,&RelationEditWidget::onEndEventChanged);
}

const FRelation &RelationEditWidget::getRelation() const
{
    return this->relation;
}

void RelationEditWidget::onPartnersListChanged(const QList<QUuid> &partnersList)
{
    this->relation.setPartners(partnersList);
    emit this->changed(this->relation);
}

void RelationEditWidget::onChildrenListChanged(const QList<QUuid> &childrenList)
{
    this->relation.setChildren(childrenList);
    emit this->changed(this->relation);
}

void RelationEditWidget::onBeginEventChanged(const FEvent &event)
{
    this->relation.setBegin(event);
    emit this->changed(this->relation);
}

void RelationEditWidget::onEndEventChanged(const FEvent &event)
{
    this->relation.setEnd(event);
    emit this->changed(this->relation);
}
