#include <QFileInfo>

#include <rbl_logger.h>
#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>

#include "session.h"

Session::Session(QObject *parent)
    : QObject(parent)
    , treeChanged(false)
{
    this->pTree = new FTree(this);

    this->fileSystemWatcher = new QFileSystemWatcher(this);

    QObject::connect(this->pTree,&FTree::changed,this,&Session::onTreeChanged);
    QObject::connect(this->pTree,&FTree::fileSaved,this,&Session::onTreeFileSaved);

    QObject::connect(this->fileSystemWatcher,&QFileSystemWatcher::fileChanged,this,&Session::onTreeFileChanged);
}

const FTree *Session::getTree() const
{
    return this->pTree;
}

FTree *Session::getTree()
{
    return this->pTree;
}

const QString &Session::getTreeFileName() const
{
    return this->treeFileName;
}

void Session::setTreeFileName(const QString &treeFileName)
{
    if (!this->treeFileName.isEmpty())
    {
        RLogger::debug("Removing file \"%s\" from file system watcher.\n",this->treeFileName.toUtf8().constData());
        this->fileSystemWatcher->removePath(this->treeFileName);
    }
    this->treeFileName = treeFileName;
    if (!this->treeFileName.isEmpty())
    {
        RLogger::debug("Adding file \"%s\" to file system watcher.\n",this->treeFileName.toUtf8().constData());
        this->fileSystemWatcher->addPath(this->treeFileName);
    }
}

bool Session::getTreeChanged() const
{
    return this->treeChanged;
}

void Session::readTreeFile()
{
    RLogger::debug("Registering to load \'%s\'\n",this->treeFileName.toUtf8().constData());
    RToolInput toolInput;
    toolInput.addAction(FToolAction::readTreeFile(this->pTree,this->treeFileName));
    RToolTask *toolTask = new RToolTask(toolInput);
    RJobManager::getInstance().submit(toolTask);
}

void Session::writeTreeFile() const
{
    RLogger::debug("Registering to write \'%s\'\n",this->treeFileName.toUtf8().constData());
    RToolInput toolInput;
    toolInput.addAction(FToolAction::writeTreeFile(this->pTree,this->treeFileName));
    RToolTask *toolTask = new RToolTask(toolInput);

    QObject::connect(toolTask,&RToolTask::actionFinished,this,&Session::writeActionFinished);
    QObject::connect(toolTask,&RToolTask::actionFailed,this,&Session::writeActionFailed);

    this->fileSystemWatcher->blockSignals(true);
    RJobManager::getInstance().submit(toolTask);
}

void Session::onTreeChanged()
{
    this->treeChanged = true;
}

void Session::onTreeFileSaved(const QString &fileName)
{
    this->setTreeFileName(fileName);
    this->treeChanged = false;
}

void Session::onTreeFileChanged(const QString &fileName)
{
    if (QFileInfo::exists(fileName))
    {
        RLogger::info("File \"%s\" has been modified outside of this application.\n",this->treeFileName.toUtf8().constData());
        emit this->treeFileChanged(fileName);
    }
    else
    {
        RLogger::info("File \"%s\" has been removed outside of this application.\n",this->treeFileName.toUtf8().constData());
        emit this->treeFileRemoved(fileName);
    }
}

void Session::writeActionFinished(const QSharedPointer<RToolAction> &action)
{
    this->fileSystemWatcher->blockSignals(false);
}

void Session::writeActionFailed(const QSharedPointer<RToolAction> &action)
{
    this->fileSystemWatcher->blockSignals(false);
}
