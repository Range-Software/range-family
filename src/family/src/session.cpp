#include <rbl_logger.h>

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
    RLogger::info("File \"%s\" has been modified outside of this application.\n",this->treeFileName.toUtf8().constData());
    emit this->treeFileChanged(fileName);
}
