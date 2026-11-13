#ifndef DIAGRAM_WIDGET_H
#define DIAGRAM_WIDGET_H

#include <QUuid>
#include <QWidget>

#include "diagram_scene.h"
#include "diagram_view.h"

class QPushButton;

class DiagramWidget : public QWidget
{

    Q_OBJECT

    protected:

        const FTree *familyTree;

        QUuid itemId;

        DiagramView *view;
        DiagramScene *scene;
        //! Zoom controls placed over the top right corner of the view.
        QWidget *controls;
        //! Export button placed over the top left corner of the view.
        QPushButton *exportButton;

    public:

        //! Constructor.
        explicit DiagramWidget(FTree *familyTree, const QUuid &itemId, QWidget *parent = nullptr);

        //! Return item ID.
        const QUuid &getItemId() const;

        //! Set new item ID.
        void setItemId(const QUuid &itemId);

    protected:

        //! Keep controls in the corner of the view when the view is resized.
        bool eventFilter(QObject *watched, QEvent *event) override;

        //! Place zoom controls to the top right and export button to the top left corner of the view.
        void updateControlsGeometry();

    protected slots:

        void onExportTriggered();

        void onItemIdActivated(const QUuid &itemId);

        void onItemIdChanged(const QUuid &itemId);

        void onScenePopulated();

        void onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

        void onScaleFactorChanged(double factor);

        //! Application style or color scheme has been applied.
        void onStyleApplied();

    signals:

        void itemIdChanged(const QUuid &itemId);

        void personSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        void relationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

};

#endif // DIAGRAM_WIDGET_H
