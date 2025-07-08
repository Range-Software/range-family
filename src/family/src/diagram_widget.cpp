#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>

#include <rbl_logger.h>

#include <rgl_application.h>

#include "diagram_widget.h"
#include "scale_controll_widget.h"

DiagramWidget::DiagramWidget(FTree *familyTree, const QUuid &itemId, QWidget *parent)
    : QWidget{parent}
    , familyTree{familyTree}
    , itemId{itemId}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QToolBar *toolBar = new QToolBar;
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mainLayout->addWidget(toolBar);

    QAction *exportAction = new QAction(QIcon(":/icons/file/pixmaps/range-import.svg"),tr("Export tree"));
    toolBar->addAction(exportAction);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    ScaleControllWidget *scaleControll = new ScaleControllWidget;
    toolBar->addWidget(scaleControll);

    this->view = new DiagramView(familyTree);
    mainLayout->addWidget(this->view);

    this->scene = new DiagramScene(familyTree,this->itemId);
    this->view->setScene(this->scene);

    QObject::connect(exportAction,&QAction::triggered,this,&DiagramWidget::onExportTriggered);

    QObject::connect(this->scene,&DiagramScene::itemIdChanged,this,&DiagramWidget::onItemIdChanged);
    QObject::connect(this->scene,&DiagramScene::populated,this,&DiagramWidget::onScenePopulated);
    QObject::connect(this->scene,&DiagramScene::personSelectionChanged,this,&DiagramWidget::onPersonSelectionChanged);
    QObject::connect(this->scene,&DiagramScene::relationSelectionChanged,this,&DiagramWidget::onRelationSelectionChanged);

    QObject::connect(this->view,&DiagramView::itemIdActivated,this,&DiagramWidget::onItemIdChanged);
    QObject::connect(this->view,&DiagramView::scaleChanged,scaleControll,&ScaleControllWidget::setScale);

    QObject::connect(scaleControll,&ScaleControllWidget::factorChanged,this,&DiagramWidget::onScaleFactorChanged);
}

void DiagramWidget::setItemId(const QUuid &itemId)
{
    if (this->itemId == itemId)
    {
        return;
    }
    this->itemId = itemId;
    this->scene->setItemId(this->itemId);
    this->view->centerOnItem(this->itemId);

    emit this->itemIdChanged(this->itemId);
}

void DiagramWidget::onExportTriggered()
{
    QString fileName = RApplication::instance()->getApplicationSettings()->getDataDir() + QDir::separator() + this->itemId.toString(QUuid::WithoutBraces) + ".xml";
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Export family tree to selected file"),
                                            fileName,
                                            tr("Family files")+ "(*.xml *.json)");
    if (!fileName.isEmpty())
    {
        RLogger::info("Write displayed family tree to file \"%s\".\n",fileName.toUtf8().constData());
        try
        {
            FTree *tree = this->scene->getTreeModel().getVisibleTree();
            tree->writeFile(fileName);
            RLogger::info("Tree has been successfully exported to file \'%s\'.\n", fileName.toUtf8().constData());
            tree->deleteLater();
        }
        catch (const RError &error)
        {
            RLogger::error("Failed to export tree to file \"%s\". %s\n",
                           fileName.toUtf8().constData(),
                           error.getMessage().toUtf8().constData());
        }
    }
}

void DiagramWidget::onItemIdChanged(const QUuid &itemId)
{
    this->setItemId(itemId);
}

void DiagramWidget::onScenePopulated()
{
    QRectF sceneRect(this->scene->sceneRect());

    this->view->centerOn(sceneRect.center());
}

void DiagramWidget::onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds)
{
    emit this->personSelectionChanged(selectedPersonsIds);
}

void DiagramWidget::onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds)
{
    emit this->relationSelectionChanged(selectedRelationsIds);
}

void DiagramWidget::onScaleFactorChanged(double factor)
{
    this->view->zoom(factor,QGraphicsView::AnchorViewCenter);
}
