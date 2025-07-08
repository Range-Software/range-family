#include <QVBoxLayout>

#include "event_edit_widget.h"

EventEditWidget::EventEditWidget(const FEvent &event, QWidget *parent)
    : QWidget(parent)
    , event(event)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->dateEdit = new DateEditWidget(this->event.getDate());
    mainLayout->addWidget(dateEdit);

    this->placeEdit = new PlaceEditWidget(this->event.getPlace());
    mainLayout->addWidget(placeEdit);

    QObject::connect(this->dateEdit,&DateEditWidget::changed,this,&EventEditWidget::onDateChanged);
    QObject::connect(this->placeEdit,&PlaceEditWidget::changed,this,&EventEditWidget::onPlaceChanged);
}

void EventEditWidget::onDateChanged(const FDate &date)
{
    this->event.setDate(date);
    emit this->changed(this->event);
}

void EventEditWidget::onPlaceChanged(const FPlace &place)
{
    this->event.setPlace(place);
    emit this->changed(this->event);
}
