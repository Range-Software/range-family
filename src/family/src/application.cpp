#include <QDir>
#include <QFileInfo>

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

    QObject::connect(this->session,&Session::treeFileLoaded,this,&Application::onTreeFileLoaded);
    QObject::connect(this->session,&Session::treeFileChanged,this,&Application::onTreeFileChanged);
    QObject::connect(this->session,&Session::treeFileRemoved,this,&Application::onTreeFileRemoved);
}

void Application::finalize()
{

}

//! Check whether given file is located at or below given directory.
static bool isBelowDirectory(const QString &fileName, const QString &directory)
{
    if (fileName.isEmpty() || directory.isEmpty())
    {
        return false;
    }

    QString relativePath = QDir(directory).relativeFilePath(QFileInfo(fileName).absolutePath());

    return !(relativePath.isEmpty() ||
             QDir::isAbsolutePath(relativePath) ||
             relativePath == ".." ||
             relativePath.startsWith("../"));
}

void Application::onTreeFileLoaded(const QString &fileName)
{
    QString dataDir = this->applicationSettings->getDataDir();

    if (isBelowDirectory(fileName,dataDir))
    {
        return;
    }

    QString question = tr("The family tree file is not stored in the data directory.")
                     + "<pre>" + fileName + "</pre>"
                     + tr("Would you like to move it to the data directory?")
                     + "<pre>" + dataDir + "</pre>";

    if (RMessageBox::question(this->mainWindow,tr("Tree file outside of the data directory"),question) != RMessageBox::Yes)
    {
        return;
    }

    QString newFileName = QDir(dataDir).absoluteFilePath(QFileInfo(fileName).fileName());

    if (QFileInfo::exists(newFileName))
    {
        QString message = tr("The data directory already contains a file with the same name.")
                        + "<pre>" + newFileName + "</pre>"
                        + tr("The family tree file was not moved.");
        RMessageBox::warning(this->mainWindow,tr("Tree file was not moved"),message);
        return;
    }

    this->session->moveTreeFile(newFileName);
}

void Application::onTreeFileChanged(const QString &fileName)
{
    QString question = tr("The family tree file was modified.")
                     + "<pre>" + fileName + "</pre>"
                     + tr("Load modified file?");
    if (RMessageBox::question(this->mainWindow,tr("Tree file has changed"),question) == RMessageBox::Yes)
    {
        this->session->setTreeFileName(fileName);
        this->session->readTreeFile();
    }
}

void Application::onTreeFileRemoved(const QString &fileName)
{
    QString question = tr("The family tree file has been removed.")
    + "<pre>" + fileName + "</pre>"
        + tr("Save current tree to file?");
    if (RMessageBox::question(this->mainWindow,tr("Tree file has been removed"),question) == RMessageBox::Yes)
    {
        this->session->setTreeFileName(fileName);
        this->session->writeTreeFile();
    }
}
