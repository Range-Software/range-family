#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QFileDialog>
#include <QPushButton>

#include <rbl_error.h>
#include <rbl_logger.h>

#include <rgl_application.h>
#include <rgl_icon.h>

#include "diagram_widget.h"
#include "scale_control_widget.h"

//! Style sheet of the controls placed over the diagram, the same look as the clear button of the output.
static const QString controlsStyleSheet =
    "QPushButton, QDoubleSpinBox, QLabel {"
    "   background-color: rgba(128, 128, 128, 60);"
    "   border: none;"
    "   border-radius: 4px;"
    "   padding: 4px 8px;"
    "}"
    // Vertical padding would leave no room for the text of the spin box.
    "QDoubleSpinBox {"
    "   padding: 0px 6px;"
    "   min-width: 4em;"
    "}"
    // Buttons beside the box step the scale, its own ones are not needed.
    "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {"
    "   background: transparent;"
    "   border: none;"
    "   width: 0px;"
    "}"
    "QPushButton:hover, QDoubleSpinBox:hover {"
    "   background-color: rgba(128, 128, 128, 180);"
    "}";

DiagramWidget::DiagramWidget(FTree *familyTree, const QUuid &itemId, QWidget *parent)
    : QWidget{parent}
    , familyTree{familyTree}
    , itemId{itemId}
{
    R_LOG_TRACE_IN;
    // Margins are given by the dialog or dock holding the diagram.
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->view = new DiagramView(familyTree);
    mainLayout->addWidget(this->view);

    // Controls float over the top corners of the diagram, so the diagram gets the whole space.
    // Export is on the left, zoom on the right.
    this->exportButton = new QPushButton(RIcon::fromFile(":/icons/file/pixmaps/range-import.svg"),tr("Export tree"),this->view);
    this->exportButton->setAutoDefault(false);
    this->exportButton->setStyleSheet(controlsStyleSheet);

    this->controls = new QWidget(this->view);
    this->controls->setStyleSheet(controlsStyleSheet);

    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->setContentsMargins(0,0,0,0);
    controlsLayout->setSpacing(5);
    this->controls->setLayout(controlsLayout);

    ScaleControlWidget *scaleControl = new ScaleControlWidget;
    controlsLayout->addWidget(scaleControl);

    this->view->installEventFilter(this);
    this->updateControlsGeometry();

    this->scene = new DiagramScene(familyTree,this->itemId);
    this->view->setScene(this->scene);
    this->view->centerOnItem(itemId);

    QObject::connect(this->exportButton,&QPushButton::clicked,this,&DiagramWidget::onExportTriggered);

    QObject::connect(this->scene,&DiagramScene::itemIdChanged,this,&DiagramWidget::onItemIdChanged);
    QObject::connect(this->scene,&DiagramScene::populated,this,&DiagramWidget::onScenePopulated);
    QObject::connect(this->scene,&DiagramScene::personSelectionChanged,this,&DiagramWidget::onPersonSelectionChanged);
    QObject::connect(this->scene,&DiagramScene::relationSelectionChanged,this,&DiagramWidget::onRelationSelectionChanged);

    QObject::connect(this->view,&DiagramView::itemIdActivated,this,&DiagramWidget::onItemIdActivated);
    QObject::connect(this->view,&DiagramView::scaleChanged,scaleControl,&ScaleControlWidget::setScale);

    QObject::connect(scaleControl,&ScaleControlWidget::factorChanged,this,&DiagramWidget::onScaleFactorChanged);

    // Items of the diagram choose their colors for the color scheme while they are painted.
    QObject::connect(RApplication::instance(),&RApplication::styleApplied,this,&DiagramWidget::onStyleApplied);

    R_LOG_TRACE_OUT;
}

bool DiagramWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->view && event->type() == QEvent::Resize)
    {
        this->updateControlsGeometry();
    }
    return this->QWidget::eventFilter(watched,event);
}

void DiagramWidget::updateControlsGeometry()
{
    // Right of the controls is left free for the vertical scroll bar.
    const QSize size = this->controls->sizeHint();
    this->controls->setGeometry(this->view->width() - 25 - size.width(),5,size.width(),size.height());
    this->controls->raise();

    // Export button gets the height of the zoom controls, so both corners look alike.
    this->exportButton->setGeometry(5,5,this->exportButton->sizeHint().width(),size.height());
    this->exportButton->raise();
}

const QUuid &DiagramWidget::getItemId() const
{
    return this->itemId;
}

void DiagramWidget::setItemId(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    if (this->itemId == itemId)
    {
        R_LOG_TRACE_OUT;
        return;
    }
    this->itemId = itemId;
    this->scene->setItemId(this->itemId);

    emit this->itemIdChanged(this->itemId);
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onExportTriggered()
{
    R_LOG_TRACE_IN;
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
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onItemIdActivated(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    this->setItemId(itemId);
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onItemIdChanged(const QUuid &itemId)
{
    R_LOG_TRACE_IN;
    this->view->centerOnItem(itemId);
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onScenePopulated()
{
    R_LOG_TRACE_IN;
    QRectF sceneRect(this->scene->sceneRect());

    this->view->centerOn(sceneRect.center());
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onPersonSelectionChanged(const QList<QUuid> &selectedPersonsIds)
{
    R_LOG_TRACE_IN;
    emit this->personSelectionChanged(selectedPersonsIds);
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onRelationSelectionChanged(const QList<QUuid> &selectedRelationsIds)
{
    R_LOG_TRACE_IN;
    emit this->relationSelectionChanged(selectedRelationsIds);
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onScaleFactorChanged(double factor)
{
    R_LOG_TRACE_IN;
    this->view->zoom(factor,QGraphicsView::AnchorViewCenter);
    R_LOG_TRACE_OUT;
}

void DiagramWidget::onStyleApplied()
{
    R_LOG_TRACE_IN;
    this->scene->update();
    this->view->viewport()->update();
    R_LOG_TRACE_OUT;
}
