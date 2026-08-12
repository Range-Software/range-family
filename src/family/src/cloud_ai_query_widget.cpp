#include <QDir>
#include <QFileInfo>

#include <rbl_logger.h>

#include <rcl_ai_query.h>
#include <rcl_file_info.h>

#include "application.h"
#include "cloud_ai_query_widget.h"

CloudAiQueryWidget::CloudAiQueryWidget(RCloudConnectionHandler *connectionHandler,
                                       RApplicationSettings *applicationSettings,
                                       QWidget *parent)
    : RCloudAiQueryWidget{connectionHandler,applicationSettings,parent}
    , historyTreeFileName{Application::instance()->getSession()->getTreeFileName()}
{
    QObject::connect(Application::instance()->getSession(),&Session::treeFileLoaded,this,&CloudAiQueryWidget::resetHistoryIfTreeChanged);
}

void CloudAiQueryWidget::buildQuery(RAIQuery &aiQuery)
{
    // The tree file is resolved for every single query. The dialog is not modal,
    // therefore a different family tree can be loaded and the file can be
    // synchronized to Cloud - and by that change its ID - at any time.

    // Loading a tree without reading a file (new, closed or renamed tree) is not signaled.
    this->resetHistoryIfTreeChanged();

    const RApplicationSettings *applicationSettings = Application::instance()->getApplicationSettings();
    const QString treeFileName = Application::instance()->getSession()->getTreeFileName();

    if (treeFileName.isEmpty())
    {
        this->appendNote(tr("The family tree has not been saved yet. The answer will not take it into account."));
        return;
    }

    const QFileInfo treeFileInfo(treeFileName);

    // Only files stored in the data directory are synchronized to Cloud.
    if (QDir(treeFileInfo.absolutePath()).canonicalPath() != QDir(applicationSettings->getDataDir()).canonicalPath())
    {
        this->appendNote(tr("The family tree file is not stored in the data directory and is therefore not synchronized to Cloud. The answer will not take it into account."));
        return;
    }

    if (!applicationSettings->getCloudSyncDataDirectory() || applicationSettings->getCloudRefreshTimeout() == 0)
    {
        this->appendNote(tr("Synchronization of the data directory with Cloud is turned off. The answer will not take the family tree into account."));
        return;
    }

    const RFileInfo fileInfo = Application::instance()->getCloudFileManager()->findRemoteFile(treeFileInfo.fileName());

    if (fileInfo.getId().isNull())
    {
        this->appendNote(tr("The family tree file has not been synchronized to Cloud yet. The answer will not take it into account."));
        return;
    }

    RLogger::info("Family tree file \'%s\' is referenced by the AI query as Cloud file \'%s\'.\n",
                  treeFileInfo.fileName().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());

    aiQuery.setFileId(fileInfo.getId());
    // File description is processed by the AI service, therefore it is not translated.
    aiQuery.setFileDescription(QStringLiteral("Family tree stored in the Range Family JSON format."));

    if (RFileInfo::findMd5Checksum(treeFileName) != fileInfo.getMd5Checksum())
    {
        this->appendNote(tr("The family tree file stored in Cloud differs from the opened one. The answer is based on the version stored in Cloud."));
    }
    else if (Application::instance()->getSession()->getTreeChanged())
    {
        this->appendNote(tr("The family tree contains unsaved changes which are not stored in Cloud. The answer is based on the version stored in Cloud."));
    }
}

void CloudAiQueryWidget::appendNote(const QString &note)
{
    this->appendMarkdown(QStringLiteral("*") + note + QStringLiteral("*"));
}

void CloudAiQueryWidget::resetHistoryIfTreeChanged()
{
    const QString treeFileName = Application::instance()->getSession()->getTreeFileName();

    if (treeFileName == this->historyTreeFileName)
    {
        return;
    }

    this->historyTreeFileName = treeFileName;

    if (this->hasHistory())
    {
        this->clearHistory();
        this->appendNote(tr("The family tree has changed. Previous questions and answers are no longer taken into account."));
    }
}
