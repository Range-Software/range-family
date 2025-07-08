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

class RProgressBar;

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

        //! Enable progress-bar autohide.
        void progressAutoHideEnable();

        //! Disable progress-bar autohide.
        void progressAutoHideDisable();

    protected:

        //! Create all menu items.
        void createMenus();

        //! Create all tool bars.
        void createToolBars();

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
