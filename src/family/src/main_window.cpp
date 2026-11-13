#include <QtGui/QCloseEvent>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QStatusBar>
#include <QSignalBlocker>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <rbl_error.h>
#include <rbl_logger.h>
#include <rbl_utils.h>
#include <rbl_job_manager.h>

#include <rfl_tool_action.h>

#include <rgl_first_run_dialog.h>
#include <rgl_message_box.h>
#include <rgl_progress_handler.h>
#include <rgl_progress_bar.h>
#include <rgl_icon.h>
#include <rgl_ribbon_bar.h>
#include <rgl_split_tool_button.h>

#include "application.h"
#include "main_window.h"
#include "action.h"
#include "ai_chat_widget.h"
#include "editor_widget.h"
#include "output_widget.h"

//! Version of tool bar arrangement. Increase it when tool bars change, to place them anew for existing users.
static constexpr int toolBarsVersion = 2;

//! Actions showing and hiding docks, indexed by MainWindow::DockType.
static const QList<Action::Type> dockActionTypes =
{
    Action::ACTION_VIEW_PERSON_DETAILS,
    Action::ACTION_VIEW_RELATIONS,
    Action::ACTION_VIEW_FAMILY_TREE,
    Action::ACTION_VIEW_OUTPUT,
    Action::ACTION_AI_CHAT
};

//! Version of the stored window state. Increase it when docks change, so a state which does not know them is not restored.
static constexpr int windowStateVersion = 1;

MainWindow::MainWindow(Session *session,
                       RCloudSessionManager *cloudSession,
                       RApplicationSettings *applicationSettings,
                       RActionList *actionList,
                       QWidget *parent)
    : QMainWindow{parent}
    , session{session}
    , cloudSession{cloudSession}
    , applicationSettings{applicationSettings}
    , actionList{actionList}
    , aiChatWidget{nullptr}
    , isFirstRun{true}
{
    R_LOG_TRACE_IN;
    if (this->objectName().isEmpty())
    {
        this->setObjectName(QString::fromUtf8("MainWindow"));
    }

    this->readSettings();

    this->applyShortcutsToActions();

    this->setWindowTitle(RVendor::name() + " :: " + tr("Family tree manager"));
    this->setWindowIcon(RIcon::fromFile(Application::windowIconFile()));

    int toolbarIconSize = this->applicationSettings->getToolbarIconSize();
    this->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    this->setToolButtonStyle(Qt::ToolButtonIconOnly);

    // Docks can be placed side by side, stacked or tabbed.
    this->setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

    this->createDockWidgets();
    this->createMenus();
    this->createToolBars();
    this->createRibbon();
    this->createStatusBar();

    // Shortcut of an action works only while a widget holding the action is visible,
    // which buttons on other pages of the ribbon are not.
    for (uint i=0;i<this->actionList->getNActions();i++)
    {
        RAction *action = this->actionList->getAction(i);
        if (action && !action->isSeparator())
        {
            this->addAction(action);
        }
    }

    this->restoreGeometry(this->applicationSettings->value("mainWindow/geometry").toByteArray());
    if (!this->restoreState(this->applicationSettings->value("mainWindow/windowState").toByteArray(),windowStateVersion))
    {
        // Sizes can be given to docks only after the window has got its own.
        QTimer::singleShot(0,this,&MainWindow::resizeDockWidgets);
    }
    this->syncDockActions();

    // Stored state knows only tool bars of the time it was saved.
    if (this->applicationSettings->value("mainWindow/toolBarsVersion",1).toInt() != toolBarsVersion)
    {
        this->placeToolBars();
        this->applicationSettings->setValue("mainWindow/toolBarsVersion",toolBarsVersion);
    }

    for (RSplitToolButton *button : std::as_const(this->toolBarSplitButtons))
    {
        button->setLastUsedPosition(this->applicationSettings->value("mainWindow/" + button->objectName(),0).toInt());
    }

    // Must follow restoring of the state, which brings back visibility of tool bars.
    const bool ribbonLayout = this->applicationSettings->value("mainWindow/ribbonLayout",false).toBool();
    {
        const QSignalBlocker ribbonLayoutActionBlocker(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
        this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT)->setChecked(ribbonLayout);
    }
    this->setRibbonLayout(ribbonLayout);

    const bool ribbonCompact = this->applicationSettings->value("mainWindow/ribbonCompact",false).toBool();
    {
        const QSignalBlocker ribbonCompactActionBlocker(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_COMPACT));
        this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_COMPACT)->setChecked(ribbonCompact);
    }
    this->ribbonBar->setCompact(ribbonCompact);

    this->ribbonBar->setCollapsed(this->applicationSettings->value("mainWindow/ribbonCollapsed",false).toBool());
    this->ribbonBar->setCurrentPage(this->applicationSettings->value("mainWindow/ribbonPage",0).toInt());

    // Toolbar icon size changed signal
    QObject::connect(this->applicationSettings,&RApplicationSettings::toolbarIconSizeChanged,this,&MainWindow::onToolbarIconSizeChanged);

    // Main progress signals
    QObject::connect(&RProgressHandler::getInstance(),&RProgressHandler::progress,this,&MainWindow::onMainProgress);
    QObject::connect(&RProgressHandler::getInstance(),&RProgressHandler::progressInitialize,this,&MainWindow::onMainProgressInitialize);
    QObject::connect(&RProgressHandler::getInstance(),&RProgressHandler::progressFinalize,this,&MainWindow::onMainProgressFinalize);

    // Job manager signals
    QObject::connect(&RJobManager::getInstance(),&RJobManager::jobBlocking,this,&MainWindow::setDisabled);

    if (this->isFirstRun)
    {
        RFirstRunDialog(this).exec();
    }

    // Load last tree file
    if (!this->session->getTreeFileName().isEmpty())
    {
        this->session->readTreeFile();
    }
    R_LOG_TRACE_OUT;
}

EditorWidget *MainWindow::getEditor() const
{
    R_LOG_TRACE;
    return this->editor;
}

void MainWindow::setDockVisible(DockType dockType, bool visible)
{
    R_LOG_TRACE_IN;
    QDockWidget *dockWidget = this->dockWidgets.at(dockType);
    // Nothing to do if the action only follows the dock.
    if (dockWidget->isHidden() == visible)
    {
        if (visible)
        {
            dockWidget->show();
            dockWidget->raise();
        }
        else if (dockType == DockAiChat && !this->tabifiedDockWidgets(dockWidget).isEmpty() && dockWidget->visibleRegion().isEmpty())
        {
            // AI chat is shown, but behind another tab. Bring it to the front instead of hiding it.
            dockWidget->raise();
            this->actionList->getAction(dockActionTypes.at(dockType))->setChecked(true);
        }
        else
        {
            dockWidget->hide();
        }
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::setRibbonLayout(bool ribbonLayout)
{
    R_LOG_TRACE_IN;
    this->classicMenuBar->setVisible(!ribbonLayout);
    this->ribbonBar->setVisible(ribbonLayout);

    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        toolBar->setVisible(!ribbonLayout);
        // Otherwise context menu could bring tool bars back to the ribbon layout.
        toolBar->toggleViewAction()->setVisible(!ribbonLayout);
    }

    this->applicationSettings->setValue("mainWindow/ribbonLayout",ribbonLayout);
    R_LOG_TRACE_OUT;
}

void MainWindow::setRibbonCompact(bool ribbonCompact)
{
    R_LOG_TRACE_IN;
    this->ribbonBar->setCompact(ribbonCompact);
    this->applicationSettings->setValue("mainWindow/ribbonCompact",ribbonCompact);

    // Asking for compact ribbon from the classic layout means asking for the ribbon as well.
    if (ribbonCompact)
    {
        this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT)->setChecked(true);
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::progressAutoHideEnable()
{
    R_LOG_TRACE;
    this->mainProgressBar->setAutoHide(true);
}

void MainWindow::progressAutoHideDisable()
{
    R_LOG_TRACE;
    this->mainProgressBar->setAutoHide(false);
}

void MainWindow::createMenus()
{
    R_LOG_TRACE_IN;
    // Menu bar and ribbon share the place above the tool bars, only one of them is visible at a time.
    QWidget *menuWidget = new QWidget(this);
    QVBoxLayout *menuWidgetLayout = new QVBoxLayout(menuWidget);
    menuWidgetLayout->setContentsMargins(0,0,0,0);
    menuWidgetLayout->setSpacing(0);
    this->setMenuWidget(menuWidget);

    QMenuBar *menuBar = new QMenuBar(menuWidget);
    menuBar->setNativeMenuBar(!this->applicationSettings->getDontUseNativeMenuBar());
    menuWidgetLayout->addWidget(menuBar);
    this->classicMenuBar = menuBar;

    menuBar->addAction(this->createFileMenu(menuBar)->menuAction());
    menuBar->addAction(this->createEditMenu(menuBar)->menuAction());
    menuBar->addAction(this->createViewMenu(menuBar)->menuAction());
    menuBar->addAction(this->createCloudMenu(menuBar)->menuAction());
    menuBar->addAction(this->createAiMenu(menuBar)->menuAction());
    menuBar->addAction(this->createHelpMenu(menuBar)->menuAction());
    R_LOG_TRACE_OUT;
}

QMenu *MainWindow::createFileMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuFile = new QMenu(parent);
    menuFile->setTitle(tr("File"));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_NEW));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_OPEN));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_SAVE));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_SAVE_AS));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_CLOSE));
    menuFile->addSeparator();
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_IMPORT));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_COMPARE));
    menuFile->addSeparator();
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_STORE_IMAGES_IN_TREE));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_STORE_IMAGES_IN_DATA_DIR));
    menuFile->addSeparator();
    QMenu *menuFileFolders = new QMenu(menuFile);
    menuFileFolders->setTitle(tr("Folders"));
    menuFileFolders->setIcon(RIcon::fromFile(":/icons/file/pixmaps/range-open.svg"));
    menuFileFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_CERT));
    menuFileFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_DATA));
    menuFileFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_DOC));
    menuFileFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_LOG));
    menuFileFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_TMP));
    menuFile->addAction(menuFileFolders->menuAction());
    menuFile->addSeparator();
    menuFile->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_COMPACT));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_SETTINGS));
    menuFile->addSeparator();
    menuFile->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_QUIT));
    R_LOG_TRACE_OUT;
    return menuFile;
}

QMenu *MainWindow::createEditMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuEdit = new QMenu(parent);
    menuEdit->setTitle(tr("Edit"));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_ADD));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_EDIT));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_REMOVE));
    menuEdit->addSeparator();
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_SHOW_TREE));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_SET_ACTIVE));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_MERGE));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_PERSON_SUGGEST_DATES));
    menuEdit->addSeparator();
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_RELATION_ADD));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_RELATION_EDIT));
    menuEdit->addAction(this->actionList->getAction(Action::ACTION_EDIT_RELATION_REMOVE));
    R_LOG_TRACE_OUT;
    return menuEdit;
}

QMenu *MainWindow::createViewMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuView = new QMenu(parent);
    menuView->setTitle(tr("View"));
    for (Action::Type type : dockActionTypes)
    {
        menuView->addAction(this->actionList->getAction(type));
    }
    R_LOG_TRACE_OUT;
    return menuView;
}

void MainWindow::createDockWidgets()
{
    R_LOG_TRACE_IN;
    // Editor shows the list of persons and owns the views kept in sync with it.
    this->editor = new EditorWidget(this->session);
    this->setCentralWidget(this->editor);

    this->outputWidget = new OutputWidget;

    this->detailsDock = this->createDockWidget("detailsDock",tr("Person details"),this->editor->getPersonDetails());
    this->relationsDock = this->createDockWidget("relationsDock",tr("Relations"),this->editor->getRelationsList());
    this->diagramDock = this->createDockWidget("diagramDock",tr("Family tree"),this->editor->getDiagram());
    this->outputDock = this->createDockWidget("outputDock",tr("Output"),this->outputWidget);
    // AI chat is created when it is shown for the first time.
    this->aiChatDock = this->createDockWidget("aiChatDock",tr("AI chat"),nullptr);

    // Person details above relations on the right, family tree and output share the bottom.
    this->addDockWidget(Qt::RightDockWidgetArea,this->detailsDock);
    this->addDockWidget(Qt::RightDockWidgetArea,this->relationsDock);
    this->addDockWidget(Qt::RightDockWidgetArea,this->aiChatDock);
    this->tabifyDockWidget(this->detailsDock,this->aiChatDock);
    this->addDockWidget(Qt::BottomDockWidgetArea,this->diagramDock);
    this->addDockWidget(Qt::BottomDockWidgetArea,this->outputDock);
    this->tabifyDockWidget(this->diagramDock,this->outputDock);
    this->aiChatDock->hide();
    this->detailsDock->raise();
    this->diagramDock->raise();

    // Actions of the view menu follow docks closed by their buttons or restored with the window state.
    for (int i=0;i<this->dockWidgets.count();i++)
    {
        RAction *action = this->actionList->getAction(dockActionTypes.at(i));
        QObject::connect(this->dockWidgets.at(i)->toggleViewAction(),&QAction::toggled,action,&QAction::setChecked);
    }
    this->syncDockActions();

    QObject::connect(this->diagramDock,&QDockWidget::visibilityChanged,this->editor,&EditorWidget::updateDiagram);
    QObject::connect(this->aiChatDock,&QDockWidget::visibilityChanged,this,&MainWindow::onAiChatDockVisibilityChanged);
    R_LOG_TRACE_OUT;
}

QDockWidget *MainWindow::createDockWidget(const QString &name, const QString &title, QWidget *widget)
{
    R_LOG_TRACE;
    QDockWidget *dockWidget = new QDockWidget(title,this);
    dockWidget->setObjectName(name);
    if (widget)
    {
        // Content of a dock touches its frame otherwise.
        QWidget *container = new QWidget;
        QVBoxLayout *containerLayout = new QVBoxLayout(container);
        containerLayout->addWidget(widget);
        dockWidget->setWidget(container);
    }
    this->dockWidgets.append(dockWidget);
    return dockWidget;
}

void MainWindow::syncDockActions()
{
    R_LOG_TRACE_IN;
    for (int i=0;i<this->dockWidgets.count();i++)
    {
        this->actionList->getAction(dockActionTypes.at(i))->setChecked(!this->dockWidgets.at(i)->isHidden());
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::resizeDockWidgets()
{
    R_LOG_TRACE_IN;
    this->resizeDocks({this->detailsDock},{this->width() * 2 / 5},Qt::Horizontal);
    this->resizeDocks({this->diagramDock},{this->height() / 3},Qt::Vertical);
    R_LOG_TRACE_OUT;
}

QMenu *MainWindow::createCloudMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuCloud = new QMenu(parent);
    menuCloud->setTitle(tr("Cloud"));
    menuCloud->addAction(this->actionList->getAction(Action::ACTION_CLOUD_SESSION_MANAGER));
    menuCloud->addAction(this->actionList->getAction(Action::ACTION_CLOUD_FILE_MANAGER));
    menuCloud->addAction(this->actionList->getAction(Action::ACTION_CLOUD_AI_QUERY));
    R_LOG_TRACE_OUT;
    return menuCloud;
}

QMenu *MainWindow::createAiMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuAi = new QMenu(parent);
    menuAi->setTitle(tr("AI"));
    menuAi->addAction(this->actionList->getAction(Action::ACTION_AI_SETTINGS_MANAGER));
    menuAi->addAction(this->actionList->getAction(Action::ACTION_AI_CHAT));
    R_LOG_TRACE_OUT;
    return menuAi;
}

QMenu *MainWindow::createHelpMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuHelp = new QMenu(parent);
    menuHelp->setTitle(tr("Help"));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_HELP));
#ifdef STORE_BUILD
    // Both the Apple App Store and the Microsoft Store forbid an application from
    // downloading and installing its own updates. Hidden as well, so a shortcut
    // assigned to the action cannot trigger it.
    this->actionList->getAction(Action::ACTION_APPLICATION_UPDATE)->setVisible(false);
#else
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_UPDATE));
#endif
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT_QT));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_LICENSE));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_PRIVACY_POLICY));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RELEASE_NOTES));
    R_LOG_TRACE_OUT;
    return menuHelp;
}

void MainWindow::createToolBars()
{
    R_LOG_TRACE_IN;

    auto action = [this](Action::Type type) -> QAction* { return this->actionList->getAction(type); };

    QToolBar *fileToolBar = this->createToolBar("mainToolBar",tr("File toolbar"));
    fileToolBar->addAction(action(Action::ACTION_FILE_NEW));
    fileToolBar->addAction(action(Action::ACTION_FILE_OPEN));
    this->addToolBarSplitButton(fileToolBar,"saveSplitButton",{action(Action::ACTION_FILE_SAVE),action(Action::ACTION_FILE_SAVE_AS)},false);
    fileToolBar->addAction(action(Action::ACTION_FILE_CLOSE));

    QToolBar *treeToolBar = this->createToolBar("treeToolBar",tr("Family tree toolbar"));
    treeToolBar->addAction(action(Action::ACTION_FILE_IMPORT));
    treeToolBar->addAction(action(Action::ACTION_FILE_COMPARE));
    this->addToolBarSplitButton(treeToolBar,"storeImagesSplitButton",{action(Action::ACTION_FILE_STORE_IMAGES_IN_TREE),
                                                                      action(Action::ACTION_FILE_STORE_IMAGES_IN_DATA_DIR)},true);

    QToolBar *editToolBar = this->createToolBar("editToolBar",tr("Persons and relations toolbar"));
    // Adding has to come before editing and removing, so these always start with adding.
    this->addToolBarSplitButton(editToolBar,"personSplitButton",{action(Action::ACTION_EDIT_PERSON_ADD),
                                                                 action(Action::ACTION_EDIT_PERSON_EDIT),
                                                                 action(Action::ACTION_EDIT_PERSON_REMOVE)},false);
    this->addToolBarSplitButton(editToolBar,"relationSplitButton",{action(Action::ACTION_EDIT_RELATION_ADD),
                                                                   action(Action::ACTION_EDIT_RELATION_EDIT),
                                                                   action(Action::ACTION_EDIT_RELATION_REMOVE)},false);
    editToolBar->addSeparator();
    editToolBar->addAction(action(Action::ACTION_EDIT_PERSON_SHOW_TREE));
    editToolBar->addAction(action(Action::ACTION_EDIT_PERSON_SET_ACTIVE));
    editToolBar->addAction(action(Action::ACTION_EDIT_PERSON_MERGE));
    editToolBar->addAction(action(Action::ACTION_EDIT_PERSON_SUGGEST_DATES));

    QToolBar *applicationToolBar = this->createToolBar("applicationToolBar",tr("Application toolbar"));

    // Last tool bar of a row takes the remaining width, this pushes its buttons to the right edge of the window.
    QWidget *spacer = new QWidget(applicationToolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    applicationToolBar->addWidget(spacer);

    applicationToolBar->addAction(action(Action::ACTION_CLOUD_FILE_MANAGER));
    applicationToolBar->addAction(action(Action::ACTION_CLOUD_AI_QUERY));
    applicationToolBar->addSeparator();
    applicationToolBar->addAction(action(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
    applicationToolBar->addAction(action(Action::ACTION_APPLICATION_SETTINGS));
    applicationToolBar->addSeparator();
    applicationToolBar->addAction(action(Action::ACTION_APPLICATION_HELP));

    // Tool bars are placed in this order.
    this->toolBars = { fileToolBar, treeToolBar, editToolBar, applicationToolBar };
    this->placeToolBars();

    R_LOG_TRACE_OUT;
}

QToolBar *MainWindow::createToolBar(const QString &name, const QString &title)
{
    R_LOG_TRACE;
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setObjectName(name);
    toolBar->setWindowTitle(title);
    // An icon size the tool bar only inherits from the main window is replaced
    // by the default one of the style whenever the style or color scheme
    // changes. Set explicitly, it is kept.
    toolBar->setIconSize(this->iconSize());
    return toolBar;
}

void MainWindow::addToolBarSplitButton(QToolBar *toolBar, const QString &name, const QList<QAction*> &actions, bool rememberLastUsed)
{
    R_LOG_TRACE_IN;
    RSplitToolButton *button = new RSplitToolButton(actions,rememberLastUsed,toolBar);
    button->setObjectName(name);
    button->setIconSize(toolBar->iconSize());
    // Tool bar keeps in sync only buttons it has created itself.
    QObject::connect(toolBar,&QToolBar::iconSizeChanged,button,&QToolButton::setIconSize);
    toolBar->addWidget(button);
    this->toolBarSplitButtons.append(button);
    R_LOG_TRACE_OUT;
}

void MainWindow::placeToolBars()
{
    R_LOG_TRACE_IN;
    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        this->removeToolBar(toolBar);
    }
    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        this->addToolBar(Qt::TopToolBarArea,toolBar);
        // Removing a tool bar hides it.
        toolBar->show();
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::createRibbon()
{
    R_LOG_TRACE_IN;
    this->ribbonBar = new RRibbonBar(this->menuWidget());
    this->menuWidget()->layout()->addWidget(this->ribbonBar);
    this->ribbonBar->setIconSize(this->applicationSettings->getToolbarIconSize());

    auto action = [this](Action::Type type) -> QAction* { return this->actionList->getAction(type); };

    // Line with tabs
    this->ribbonBar->addLeadingMenu(tr("File"),this->createFileMenu(this->ribbonBar));
    this->ribbonBar->addLeadingMenu(tr("View"),this->createViewMenu(this->ribbonBar));
    this->ribbonBar->addLeadingAction(action(Action::ACTION_FILE_SAVE));

    this->ribbonBar->addTrailingMenu(tr("Cloud"),action(Action::ACTION_CLOUD_FILE_MANAGER)->icon(),this->createCloudMenu(this->ribbonBar));
    this->ribbonBar->addTrailingAction(action(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
    this->ribbonBar->addTrailingAction(action(Action::ACTION_APPLICATION_RIBBON_COMPACT));
    this->ribbonBar->addTrailingAction(action(Action::ACTION_APPLICATION_SETTINGS));
    this->ribbonBar->addTrailingMenu(tr("Help"),action(Action::ACTION_APPLICATION_HELP)->icon(),this->createHelpMenu(this->ribbonBar));

    RRibbonGroup *group = nullptr;

    // Family tree
    const int treePage = this->ribbonBar->addPage(tr("Family tree"));

    group = this->ribbonBar->addGroup(treePage,tr("File"));
    group->addLargeAction(action(Action::ACTION_FILE_NEW));
    group->addLargeAction(action(Action::ACTION_FILE_OPEN));
    group->addLargeSplitAction(action(Action::ACTION_FILE_SAVE),{action(Action::ACTION_FILE_SAVE),action(Action::ACTION_FILE_SAVE_AS)});
    group->addLargeAction(action(Action::ACTION_FILE_CLOSE));

    group = this->ribbonBar->addGroup(treePage,tr("Other family trees"));
    group->addLargeAction(action(Action::ACTION_FILE_IMPORT));
    group->addLargeAction(action(Action::ACTION_FILE_COMPARE));

    group = this->ribbonBar->addGroup(treePage,tr("Images"));
    group->addSmallAction(action(Action::ACTION_FILE_STORE_IMAGES_IN_TREE),tr("Store in family tree"));
    group->addSmallAction(action(Action::ACTION_FILE_STORE_IMAGES_IN_DATA_DIR),tr("Store in data directory"));

    // Persons
    const int personsPage = this->ribbonBar->addPage(tr("Persons"));

    group = this->ribbonBar->addGroup(personsPage,tr("Person"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_ADD),tr("Add"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_EDIT),tr("Edit"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_REMOVE),tr("Remove"));

    group = this->ribbonBar->addGroup(personsPage,tr("Tools"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_SHOW_TREE),tr("Family tree"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_SET_ACTIVE),tr("Set as active"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_MERGE),tr("Merge"));
    group->addLargeAction(action(Action::ACTION_EDIT_PERSON_SUGGEST_DATES),tr("Suggest dates"));

    // Relations
    const int relationsPage = this->ribbonBar->addPage(tr("Relations"));

    group = this->ribbonBar->addGroup(relationsPage,tr("Relation"));
    group->addLargeAction(action(Action::ACTION_EDIT_RELATION_ADD),tr("Add"));
    group->addLargeAction(action(Action::ACTION_EDIT_RELATION_EDIT),tr("Edit"));
    group->addLargeAction(action(Action::ACTION_EDIT_RELATION_REMOVE),tr("Remove"));

    group = this->ribbonBar->addGroup(relationsPage,tr("Tools"));
    group->addLargeAction(action(Action::ACTION_EDIT_RELATION_SHOW_TREE),tr("Family tree"));

    // Cloud and AI
    const int cloudPage = this->ribbonBar->addPage(tr("Cloud and AI"));

    group = this->ribbonBar->addGroup(cloudPage,tr("Cloud"));
    group->addLargeAction(action(Action::ACTION_CLOUD_SESSION_MANAGER),tr("Session manager"));
    group->addLargeAction(action(Action::ACTION_CLOUD_FILE_MANAGER),tr("File manager"));
    group->addLargeAction(action(Action::ACTION_CLOUD_AI_QUERY),tr("AI query"));

    group = this->ribbonBar->addGroup(cloudPage,tr("AI"));
    group->addLargeAction(action(Action::ACTION_AI_CHAT),tr("Chat"));
    group->addLargeAction(action(Action::ACTION_AI_SETTINGS_MANAGER),tr("Settings manager"));

    R_LOG_TRACE_OUT;
}

void MainWindow::createStatusBar()
{
    R_LOG_TRACE_IN;
    QStatusBar *statusbar;

    statusbar = new QStatusBar(this);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    this->setStatusBar(statusbar);

    QWidget *progressBarWidget = new QWidget;
    statusbar->addWidget(progressBarWidget,1);

    QVBoxLayout *progressBarLayout = new QVBoxLayout;
    progressBarLayout->setSpacing(0);
    progressBarLayout->setContentsMargins(0,0,0,0);
    progressBarWidget->setLayout(progressBarLayout);

    this->mainProgressBar = new RProgressBar(this);
    this->mainProgressBar->hide();
    progressBarLayout->addWidget(this->mainProgressBar);

    // Tells about warnings and errors printed to the output while it is not visible.
    this->outputButton = new QToolButton;
    this->outputButton->setAutoRaise(true);
    this->outputButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->outputButton->setToolTip(tr("Show application output"));
    this->outputButton->hide();
    statusbar->addPermanentWidget(this->outputButton);

    QObject::connect(this->outputButton,&QToolButton::clicked,this,&MainWindow::onOutputButtonClicked);
    QObject::connect(this->outputWidget,&OutputWidget::unseenChanged,this,&MainWindow::onOutputUnseenChanged);
    R_LOG_TRACE_OUT;
}

void MainWindow::applyShortcutsToActions()
{
    R_LOG_TRACE_IN;
    for (uint i=0;i<this->actionList->getNActions();i++)
    {
        RAction *action = this->actionList->getAction(i);
        if (!action || action->isSeparator())
        {
            continue;
        }
        QString key = "shortcuts/shortcut_" + action->getName();
        if (this->applicationSettings->contains(key))
        {
            action->setShortcut(this->applicationSettings->value(key).toString());
        }
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::readSettings()
{
    R_LOG_TRACE_IN;
    this->isFirstRun = (this->applicationSettings->getStoredVersion() != RVendor::version());
    if (this->applicationSettings->getStoredVersion() <= RVendor::version())
    {
        this->session->setTreeFileName(this->applicationSettings->value("session/treeFile").toString());
        this->session->setActivePersonId(this->applicationSettings->value("session/activePersonId").toUuid());
        this->session->setLastAiChatAgent(this->applicationSettings->value("session/lastAiAgent").toString());
        this->session->setLastAiChatLanguage(this->applicationSettings->value("session/lastAiChatLanguage").toString());
    }
    this->applicationSettings->sync();
    R_LOG_TRACE_OUT;
}

void MainWindow::writeSettings() const
{
    R_LOG_TRACE_IN;
    // Main window
    this->applicationSettings->setValue("mainWindow/geometry", this->saveGeometry());
    this->applicationSettings->setValue("mainWindow/windowState", this->saveState(windowStateVersion));
    this->applicationSettings->setValue("mainWindow/ribbonCollapsed", this->ribbonBar->isCollapsed());
    this->applicationSettings->setValue("mainWindow/ribbonPage", this->ribbonBar->getCurrentPage());
    for (const RSplitToolButton *button : std::as_const(this->toolBarSplitButtons))
    {
        if (button->getRememberLastUsed())
        {
            this->applicationSettings->setValue("mainWindow/" + button->objectName(), button->getLastUsedPosition());
        }
    }

    // Shortcuts
    for (uint i=0;i<this->actionList->getNActions();i++)
    {
        const RAction *action = this->actionList->getAction(i);
        if (!action || action->isSeparator())
        {
            continue;
        }
        QString key = "shortcuts/shortcut_" + action->getName();
        this->applicationSettings->setValue(key, action->shortcut().toString());
    }

    this->applicationSettings->setValue("session/treeFile",this->session->getTreeFileName());
    this->applicationSettings->setValue("session/activePersonId",this->session->getActivePersonId());
    this->applicationSettings->setValue("session/lastAiAgent",this->session->getLastAiChatAgent());
    this->applicationSettings->setValue("session/lastAiChatLanguage",this->session->getLastAiChatLanguage());
    this->applicationSettings->sync();
    R_LOG_TRACE_OUT;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    R_LOG_TRACE_IN;

    QString quitQuestion;
    if (this->session && this->session->getTreeChanged())
    {
        quitQuestion = "<b>" + tr("The current family tree contains unsaved changes.") + "</b><br/>";
    }

    if (RMessageBox::quit(this,quitQuestion) == RMessageBox::Yes)
    {
        this->writeSettings();
        // Accept event -> close window.
        event->accept();
    }
    else
    {
        // Ignore event -> leave window open.
        event->ignore();
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::progressBar(RProgressBar *progressBar, double fraction)
{
    R_LOG_TRACE;
    progressBar->setValue(qRound(fraction*100));
//    progressBar->show();
}

void MainWindow::progressBarInitialize(RProgressBar *progressBar, const QString &message, bool pulseType)
{
    R_LOG_TRACE_IN;
    if (pulseType)
    {
        progressBar->startPulse();
    }
    else
    {
        progressBar->setRange(0,100);
    }
    progressBar->setMessage(message);
    progressBar->setValue(0);
    progressBar->show();
    this->statusBar()->clearMask();
    R_LOG_TRACE_OUT;
}

void MainWindow::progressBarFinalize(RProgressBar *progressBar, const QString &message)
{
    R_LOG_TRACE_IN;
    progressBar->stopPulse();
    progressBar->setRange(0,100);
    progressBar->setValue(100);
    progressBar->setMessage("");
    progressBar->hide();
    this->statusBar()->showMessage(message,5000);
    R_LOG_TRACE_OUT;
}

void MainWindow::setDisabled(bool disabled)
{
    R_LOG_TRACE_IN;
    this->QWidget::setDisabled(disabled);
    this->actionList->setDisabled(disabled);
    if (!disabled)
    {
        // Enabling the whole list has enabled also actions which need a selection.
        this->editor->getPersonsList()->updateActionsAvailability();
        this->editor->getRelationsList()->updateActionsAvailability();
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::onMainProgress(double fraction)
{
    R_LOG_TRACE;
    this->progressBar(this->mainProgressBar,fraction);
}

void MainWindow::onMainProgressInitialize(const QString &message, bool pulseType)
{
    R_LOG_TRACE;
    this->progressBarInitialize(this->mainProgressBar,message,pulseType);
}

void MainWindow::onMainProgressFinalize(const QString &message)
{
    R_LOG_TRACE;
    this->progressBarFinalize(this->mainProgressBar,message);
}

void MainWindow::onOutputUnseenChanged(int nWarnings, int nErrors)
{
    R_LOG_TRACE_IN;
    if (nErrors > 0)
    {
        this->outputButton->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxCritical));
        this->outputButton->setText(tr("Errors") + ": " + QLocale().toString(nErrors));
    }
    else if (nWarnings > 0)
    {
        this->outputButton->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxWarning));
        this->outputButton->setText(tr("Warnings") + ": " + QLocale().toString(nWarnings));
    }
    this->outputButton->setVisible(nErrors > 0 || nWarnings > 0);
    R_LOG_TRACE_OUT;
}

void MainWindow::onOutputButtonClicked()
{
    R_LOG_TRACE_IN;
    this->outputDock->show();
    this->outputDock->raise();
    R_LOG_TRACE_OUT;
}

void MainWindow::onAiChatDockVisibilityChanged(bool visible)
{
    R_LOG_TRACE_IN;
    if (visible && !this->aiChatWidget)
    {
        this->aiChatWidget = new AiChatWidget(Application::instance()->getAiAgentSettingsManager(),this->applicationSettings);
        this->aiChatDock->setWidget(this->aiChatWidget);
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::onToolbarIconSizeChanged(int toolbarIconSize)
{
    R_LOG_TRACE;
    this->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    // Tool bars have their icon size set explicitly, so they do not follow the
    // main window.
    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        toolBar->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    }
    this->ribbonBar->setIconSize(toolbarIconSize);
}
