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

class AiChatWidget;
class CloudAiQueryWidget;
class EditorWidget;
class OutputWidget;
class QToolButton;
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
        //! Editor of the family tree, keeping its views in sync.
        EditorWidget *editor;
        //! Application output.
        OutputWidget *outputWidget;
        //! AI chat, null until it is shown for the first time.
        AiChatWidget *aiChatWidget;
        //! Cloud AI query, null until it is shown for the first time.
        CloudAiQueryWidget *cloudAiQueryWidget;
        //! Dock with person details.
        QDockWidget *detailsDock;
        //! Dock with list of relations.
        QDockWidget *relationsDock;
        //! Dock with list of persons.
        QDockWidget *personsDock;
        //! Dock with application output.
        QDockWidget *outputDock;
        //! Dock with AI chat.
        QDockWidget *aiChatDock;
        //! Dock with Cloud AI query.
        QDockWidget *cloudAiQueryDock;
        //! List of docks indexed by DockType.
        QList<QDockWidget*> dockWidgets;
        //! Status bar button telling about unseen warnings and errors in the output.
        QToolButton *outputButton;
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

        //! Docks, in order of the view menu.
        enum DockType
        {
            DockPersons = 0,
            DockPersonDetails,
            DockRelations,
            DockOutput,
            DockAiChat,
            DockCloudAiQuery
        };

        //! Constructor.
        explicit MainWindow(Session *session,
                            RCloudSessionManager *cloudSession,
                            RApplicationSettings *applicationSettings,
                            RActionList *actionList,
                            QWidget *parent = nullptr);

        //! Return editor with views of the family tree.
        EditorWidget *getEditor() const;

        //! Show or hide given dock. Dock which gets shown is brought to the front of its tabs.
        void setDockVisible(DockType dockType, bool visible);

        //! Place docks and tool bars to their default positions and sizes.
        void restoreDefaultLayout();

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

        //! Create menu showing and hiding docks.
        QMenu *createViewMenu(QWidget *parent);

        //! Create editor, output and docks and place them to their default positions. Family tree diagram is the central widget.
        void createDockWidgets();

        //! Place docks to their default positions: persons tabbed with relations above person details on the left,
        //! AI chat tabbed with Cloud AI query on the right and output at the bottom. AI chat, Cloud AI query and output are hidden.
        void placeDockWidgets();

        //! Check actions of the view menu for docks which are shown.
        void syncDockActions();

        //! Create dock with given widget.
        QDockWidget *createDockWidget(const QString &name, const QString &title, QWidget *widget);

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

        //! Give docks their default sizes.
        void resizeDockWidgets();

        //! Number of warnings and errors not seen in the output has changed.
        void onOutputUnseenChanged(int nWarnings, int nErrors);

        //! Status bar button of the output has been clicked.
        void onOutputButtonClicked();

        //! AI chat dock has been shown or hidden.
        void onAiChatDockVisibilityChanged(bool visible);

        //! Cloud AI query dock has been shown or hidden.
        void onCloudAiQueryDockVisibilityChanged(bool visible);

        //! Toolbar icon size has changed.
        void onToolbarIconSizeChanged(int toolbarIconSize);

};

#endif /* MAIN_WINDOW_H */
