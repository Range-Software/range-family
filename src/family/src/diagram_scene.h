#ifndef DIAGRAM_SCENE_H
#define DIAGRAM_SCENE_H

#include <QGraphicsScene>

#include <rfl_tree_model.h>

class DiagramScene : public QGraphicsScene
{

    Q_OBJECT

    protected:

        const FTree *familyTree;
        QUuid itemId;
        FTreeModel treeModel;

    public:

        //! Constructor.
        explicit DiagramScene(const FTree *familyTree, const QUuid &itemId, QObject *parent = nullptr);

        //! Set new item ID.
        void setItemId(const QUuid &itemId);

        //! Return const reference to treeModel.
        const FTreeModel &getTreeModel() const;

    protected:

        void populate();

    protected slots:

        //! Person was added.
        void onPersonAdded(const FPerson &);

        //! Person was changed.
        void onPersonChanged(const FPerson &);

        //! Person was removed.
        void onPersonRemoved(QUuid);

        //! Relation was added.
        void onRelationAdded(const FRelation &);

        //! Relation was changed.
        void onRelationChanged(const FRelation &);

        //! Relation was removed.
        void onRelationRemoved(QUuid);

        //! File loaded.
        void onFileLoaded(const QString &);

        //! Selection changed.
        void onSelectionChanged();

    signals:

        //! Item ID has changed.
        void itemIdChanged(const QUuid &itemId);

        //! Person selection has changed.
        void personSelectionChanged(const QList<QUuid> &selectedPersonsIds);

        //! Relation selection has changed.
        void relationSelectionChanged(const QList<QUuid> &selectedRelationsIds);

        //! Scene was populated.
        void populated();

};

#endif // DIAGRAM_SCENE_H
