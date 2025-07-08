#ifndef EVENT_EDIT_WIDGET_H
#define EVENT_EDIT_WIDGET_H

#include <QWidget>

#include <rfl_event.h>

#include "date_edit_widget.h"
#include "place_edit_widget.h"

class EventEditWidget : public QWidget
{

    Q_OBJECT

    protected:

        FEvent event;

        DateEditWidget *dateEdit;
        PlaceEditWidget *placeEdit;

    public:

        explicit EventEditWidget(const FEvent &event, QWidget *parent = nullptr);

    protected slots:

        void onDateChanged(const FDate &date);
        void onPlaceChanged(const FPlace &place);

    signals:

        void changed(const FEvent &event);

};

#endif // EVENT_EDIT_WIDGET_H
