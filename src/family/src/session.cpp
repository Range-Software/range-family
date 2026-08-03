#include <QFile>
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
    QObject::connect(this->pTree,&FTree::fileLoaded,this,&Session::onTreeFileLoaded);
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

const QUuid &Session::getActivePersonId() const
{
    return this->activePersonId;
}

void Session::setActivePersonId(const QUuid &activePersonId)
{
    if (this->activePersonId != activePersonId)
    {
        this->activePersonId = activePersonId;
        emit this->activePersonIdChanged(activePersonId);
    }
}

const QString &Session::getLastAiChatAgent() const
{
    return this->lastAiChatAgent;
}

void Session::setLastAiChatAgent(const QString &lastAiAgent)
{
    this->lastAiChatAgent = lastAiAgent;
}

const QString &Session::getLastAiChatLanguage() const
{
    return this->lastAiChatLanguage;
}

void Session::setLastAiChatLanguage(const QString &lastAiChatLanguage)
{
    this->lastAiChatLanguage = lastAiChatLanguage;
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

void Session::moveTreeFile(const QString &newTreeFileName)
{
    RLogger::info("Moving tree file \'%s\' to \'%s\'\n",
                  this->treeFileName.toUtf8().constData(),
                  newTreeFileName.toUtf8().constData());

    // The file the tree was read from is removed once it has been written under its new name.
    this->treeFileToRemove = this->treeFileName;

    this->setTreeFileName(newTreeFileName);
    this->writeTreeFile();
}

void Session::addObsoletePictureFile(const QString &fileName)
{
    if (fileName.isEmpty() || this->obsoletePictureFiles.contains(fileName))
    {
        return;
    }
    this->obsoletePictureFiles.append(fileName);
}

void Session::clearObsoletePictureFiles()
{
    this->obsoletePictureFiles.clear();
}

void Session::removeObsoletePictureFiles()
{
    if (this->obsoletePictureFiles.isEmpty())
    {
        return;
    }

    QStringList referencedFiles;
    const QList<QUuid> personIds = this->pTree->getPersons();
    for (const QUuid &personId : personIds)
    {
        QString url = this->pTree->findPerson(personId).getPicture().getUrl();
        if (!url.isEmpty())
        {
            referencedFiles.append(url);
        }
    }

    for (const QString &fileName : std::as_const(this->obsoletePictureFiles))
    {
        // A picture may have been moved back to the file in the meantime.
        if (referencedFiles.contains(fileName) || !QFileInfo::exists(fileName))
        {
            continue;
        }

        if (QFile::remove(fileName))
        {
            RLogger::info("Picture file \"%s\" was removed.\n",fileName.toUtf8().constData());
        }
        else
        {
            RLogger::warning("Failed to remove picture file \"%s\".\n",fileName.toUtf8().constData());
        }
    }

    this->obsoletePictureFiles.clear();
}

void Session::onTreeChanged()
{
    this->treeChanged = true;

    if (!this->activePersonId.isNull())
    {
        if (!this->pTree->containsPerson(this->activePersonId))
        {
            this->setActivePersonId(QUuid());
        }
    }
}

void Session::onTreeFileLoaded(const QString &fileName)
{
    // Pending picture files belong to the tree which has just been replaced.
    this->clearObsoletePictureFiles();

    emit this->treeFileLoaded(fileName);
}

void Session::onTreeFileSaved(const QString &fileName)
{
    // Point picture urls at the files which were written next to the saved tree.
    this->pTree->rebasePictureUrls(fileName);
    this->setTreeFileName(fileName);
    this->treeChanged = false;

    // Picture data of the pictures which were moved into the tree are stored
    // in the saved file - the files they were stored in before can be removed.
    this->removeObsoletePictureFiles();

    if (!this->treeFileToRemove.isEmpty())
    {
        if (QFileInfo(this->treeFileToRemove) != QFileInfo(fileName))
        {
            if (QFile::remove(this->treeFileToRemove))
            {
                RLogger::info("Tree file \"%s\" was moved to \"%s\".\n",
                              this->treeFileToRemove.toUtf8().constData(),
                              fileName.toUtf8().constData());
            }
            else
            {
                RLogger::warning("Failed to remove tree file \"%s\".\n",this->treeFileToRemove.toUtf8().constData());
            }
        }
        this->treeFileToRemove.clear();
    }
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
    // The tree was not written - the file it was read from must be kept.
    this->treeFileToRemove.clear();

    this->fileSystemWatcher->blockSignals(false);
}
