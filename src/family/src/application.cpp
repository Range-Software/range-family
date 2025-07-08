#include <rbl_tool_task.h>
#include <rbl_job_manager.h>

#include <rfl_tool_action.h>

#include <rgl_message_box.h>

#include "application.h"
#include "action.h"
#include "main_window.h"

Application::Application(int &argc, char **argv)
    : RApplication{argc,argv}
{
    // Set translator names
    this->translatorNames << "family";
    this->translatorNames << "range-family-lib";
    this->translatorNames << "range-gui-lib";
    this->translatorNames << "range-cloud-lib";
}

Application *Application::instance() noexcept
{
    return qobject_cast<Application*>(RApplication::instance());
}

const Session *Application::getSession() const
{
    return this->session;
}

Session *Application::getSession()
{
    return this->session;
}

void Application::initialize()
{
    // Initialize action list
    this->actionList = new RActionList(Action::generateActionDefinitionList(),this);

    // Initialize session
    this->session = new Session(this);

    // Prepare main window
    this->mainWindow = new MainWindow(this->session,this->cloudSessionManager,this->applicationSettings,this->actionList);
    this->cloudConnectionHandler->setMessageBoxParentWidget(this->mainWindow);
    this->mainWindow->show();

    QObject::connect(this->session,&Session::treeFileChanged,this,&Application::onTreeFileChanged);
}

void Application::finalize()
{

}

void Application::onTreeFileChanged(const QString &fileName)
{
    QString question = tr("Family tree file was modified.")
                     + "<pre>" + fileName + "</pre>"
                     + tr("Load modified file?");
    if (RMessageBox::question(this->mainWindow,tr("Tree file has changed"),question) == RMessageBox::Yes)
    {
        this->session->setTreeFileName(fileName);
        RToolInput toolInput;
        toolInput.addAction(FToolAction::readTreeFile(Application::instance()->getSession()->getTree(),fileName));
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);
    }
}
