#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDockWidget>
#include <QToolBar>
#include <QMainWindow>
#include <QLabel>

#include <rcl_cloud_session_manager.h>

#include <rgl_application_settings.h>
#include <rgl_action_list.h>

#include "session.h"

class CentralWidget;
class EditorWidget;
class QMenu;
class QMenuBar;
class RProgressBar;
class RRibbonBar;
class RSplitToolButton;

class MainWindow : public QMainWindow
{

    Q_OBJECT

    private:

        //! Session.
        Session *session;
        //! Cloud cloudSession.
        RCloudSessionManager *cloudSession;
        //! Application settings.
        RApplicationSettings *applicationSettings;
        //! List of actions.
        RActionList *actionList;
        //! Central widget.
        CentralWidget *centralWidget;
        //! List of tool bars in order of their default placement.
        QList<QToolBar*> toolBars;
        //! Drop-down buttons on tool bars.
        QList<RSplitToolButton*> toolBarSplitButtons;
        //! Menu bar shown in the layout with menus and tool bars.
        QMenuBar *classicMenuBar;
        //! Ribbon shown in the ribbon layout.
        RRibbonBar *ribbonBar;
        //! Progress bar.
        RProgressBar* mainProgressBar;
        //! First run flag.
        bool isFirstRun;

    public:

        //! Constructor.
        explicit MainWindow(Session *session,
                            RCloudSessionManager *cloudSession,
                            RApplicationSettings *applicationSettings,
                            RActionList *actionList,
                            QWidget *parent = nullptr);

        //! Return editor with lists of persons and relations.
        EditorWidget *getEditor() const;

        //! Switch between ribbon layout and layout with menus and tool bars and remember the choice.
        void setRibbonLayout(bool ribbonLayout);

        //! Switch between compact and regular ribbon and remember the choice.
        void setRibbonCompact(bool ribbonCompact);

        //! Enable progress-bar autohide.
        void progressAutoHideEnable();

        //! Disable progress-bar autohide.
        void progressAutoHideDisable();

    protected:

        //! Create all menu items.
        void createMenus();

        //! Create file menu.
        QMenu *createFileMenu(QWidget *parent);

        //! Create menu of actions working with persons and relations.
        QMenu *createEditMenu(QWidget *parent);

        //! Create cloud menu.
        QMenu *createCloudMenu(QWidget *parent);

        //! Create AI menu.
        QMenu *createAiMenu(QWidget *parent);

        //! Create help menu.
        QMenu *createHelpMenu(QWidget *parent);

        //! Create all tool bars.
        void createToolBars();

        //! Create tool bar.
        QToolBar *createToolBar(const QString &name, const QString &title);

        //! Add drop-down button with given actions to given tool bar.
        void addToolBarSplitButton(QToolBar *toolBar, const QString &name, const QList<QAction*> &actions, bool rememberLastUsed);

        //! Place tool bars to their default positions.
        void placeToolBars();

        //! Create ribbon.
        void createRibbon();

        //! Create status bar.
        void createStatusBar();

        //! Apply shortcuts to actions.
        void applyShortcutsToActions();

        //! Read application settings.
        void readSettings();

        //! Write application settings.
        void writeSettings() const;

        //! Overloaded closeEvent function to catch close event.
        void closeEvent(QCloseEvent *event);

        //! Set progress fraction bar.
        void progressBar(RProgressBar *progressBar, double fraction);

        //! Initialize progress bar.
        void progressBarInitialize(RProgressBar *progressBar, const QString &message, bool pulseType);

        //! Finalize progress bar.
        void progressBarFinalize(RProgressBar *progressBar, const QString &message);

    protected slots:

        //! Disable whole window.
        void setDisabled(bool disabled);

        //! Set main progress fraction.
        void onMainProgress(double fraction);

        //! Initialize main progress.
        void onMainProgressInitialize(const QString &message, bool pulseType);

        //! Finalize main progress.
        void onMainProgressFinalize(const QString &message);

        //! Toolbar icon size has changed.
        void onToolbarIconSizeChanged(int toolbarIconSize);

};

#endif /* MAIN_WINDOW_H */
