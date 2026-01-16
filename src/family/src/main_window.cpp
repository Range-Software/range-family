#include <QtGui/QCloseEvent>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QStatusBar>
#include <QSplitter>
#include <QScrollBar>
#include <QHBoxLayout>

#include <rbl_error.h>
#include <rbl_logger.h>
#include <rbl_utils.h>
#include <rbl_job_manager.h>

#include <rfl_tool_action.h>

#include <rgl_first_run_dialog.h>
#include <rgl_message_box.h>
#include <rgl_progress_handler.h>
#include <rgl_progress_bar.h>

#include "main_window.h"
#include "action.h"
#include "central_widget.h"

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
    this->setWindowIcon(QIcon(":/icons/logos/pixmaps/range-family.svg"));

    int toolbarIconSize = this->applicationSettings->getToolbarIconSize();
    this->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    this->setToolButtonStyle(Qt::ToolButtonIconOnly);

    this->setDockOptions(QMainWindow::ForceTabbedDocks);

    this->createMenus();
    this->createToolBars();
    this->createStatusBar();

    CentralWidget *centralWidget = new CentralWidget(this->session);
    this->setCentralWidget(centralWidget);

    this->restoreGeometry(this->applicationSettings->value("mainWindow/geometry").toByteArray());
    this->restoreState(this->applicationSettings->value("mainWindow/windowState").toByteArray());

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
    QMenuBar *menubar = new QMenuBar(this);
    this->setMenuBar(menubar);

    QMenu *menuFile = new QMenu(menubar);
    menuFile->setTitle(QApplication::translate("MainWindow", "File"));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_NEW));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_OPEN));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_SAVE));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_SAVE_AS));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_CLOSE));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_SEPARATOR));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_IMPORT));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_FILE_COMPARE));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_SEPARATOR));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_SETTINGS));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_SEPARATOR));
    menuFile->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_QUIT));
    menubar->addAction(menuFile->menuAction());

    QMenu *menuCloud = new QMenu(menubar);
    menuCloud->setTitle(QApplication::translate("MainWindow", "Cloud"));
    menuCloud->addAction(this->actionList->getAction(Action::ACTION_CLOUD_SESSION_MANAGER));
    menuCloud->addAction(this->actionList->getAction(Action::ACTION_CLOUD_FILE_MANAGER));
    menubar->addAction(menuCloud->menuAction());

    QMenu *menuHelp = new QMenu(menubar);
    menuHelp->setTitle(QApplication::translate("MainWindow", "Help"));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_HELP));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_UPDATE));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT_QT));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_LICENSE));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RELEASE_NOTES));
    menubar->addAction(menuHelp->menuAction());
    R_LOG_TRACE_OUT;
}

void MainWindow::createToolBars()
{
    R_LOG_TRACE_IN;

    QToolBar *mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mainToolBar->setWindowTitle(tr("Main toolbar"));
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);

    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_NEW));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_OPEN));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_SAVE));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_SAVE_AS));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_CLOSE));

    mainToolBar->addSeparator();

    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_IMPORT));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_FILE_COMPARE));

    mainToolBar->addSeparator();

    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_QUIT));

    mainToolBar->addSeparator();

    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_CLOUD_FILE_MANAGER));

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainToolBar->addWidget(spacer);

    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_SETTINGS));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_SEPARATOR));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_HELP));
    mainToolBar->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT));

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
    }
    this->applicationSettings->sync();
    R_LOG_TRACE_OUT;
}

void MainWindow::writeSettings() const
{
    R_LOG_TRACE_IN;
    // Main window
    this->applicationSettings->setValue("mainWindow/geometry", this->saveGeometry());
    this->applicationSettings->setValue("mainWindow/windowState", this->saveState());

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
    this->applicationSettings->sync();
    R_LOG_TRACE_OUT;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    R_LOG_TRACE_IN;

    QString quitQuestion;
    if (this->session && this->session->getTreeChanged())
    {
        quitQuestion = "<b>" + tr("Current family tree contains unsaved changes.") + "</b><br/>";
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

void MainWindow::onToolbarIconSizeChanged(int toolbarIconSize)
{
    R_LOG_TRACE;
    this->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
}
