#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include <rbl_error.h>
#include <rbl_logger.h>
#include <rbl_utils.h>

#include <rfl_tool_action.h>

#include <rgl_application_settings_dialog.h>
#include <rgl_cloud_file_manager_dialog.h>
#include <rgl_cloud_session_dialog.h>
#include <rgl_help_dialog.h>
#include <rgl_message_box.h>
#include <rgl_software_manager_dialog.h>
#include <rgl_text_browser_dialog.h>

#include "application.h"
#include "action.h"
#include "tree_diff_dialog.h"

Action::Action(RAction::Definition definition, QObject *parent)
    : RAction{definition,parent}
{
}

QString Action::getGroupName(GroupType groupType)
{
    switch (groupType)
    {
        case ACTION_GROUP_APPLICATION:
            return QObject::tr("Application");
        case ACTION_GROUP_FILE:
            return QObject::tr("File");
        case ACTION_GROUP_VIEW:
            return QObject::tr("View");
        case ACTION_GROUP_EDIT:
            return QObject::tr("Edit");
        case ACTION_GROUP_DEBUG:
            return QObject::tr("Debug");
        case ACTION_GROUP_CLOUD:
            return QObject::tr("Cloud");
        default:
            return QString();
    }
}

QString Action::getName(Type type)
{
    switch (type)
    {
        case ACTION_APPLICATION_SETTINGS:       return "application-settings";
        case ACTION_APPLICATION_UPDATE:         return "application-update";
        case ACTION_APPLICATION_ABOUT:          return "application-about";
        case ACTION_APPLICATION_ABOUT_QT:       return "application-about_qt";
        case ACTION_APPLICATION_LICENSE:        return "application-license";
        case ACTION_APPLICATION_RELEASE_NOTES:  return "application-release_notes";
        case ACTION_APPLICATION_HELP:           return "application-help";
        case ACTION_APPLICATION_QUIT:           return "application-quit";
        case ACTION_FILE_NEW:                   return "file-new";
        case ACTION_FILE_OPEN:                  return "file-open";
        case ACTION_FILE_IMPORT:                return "file-import";
        case ACTION_FILE_COMPARE:               return "file-compare";
        case ACTION_FILE_SAVE:                  return "file-save";
        case ACTION_FILE_SAVE_AS:               return "file-save_as";
        case ACTION_FILE_CLOSE:                 return "file-close";
        case ACTION_CLOUD_SESSION_MANAGER:      return "cloud-sessiion_manager";
        case ACTION_CLOUD_FILE_MANAGER:         return "cloud-file_manager";
        default: return QString();
    }
}

QList<Action::GroupType> Action::getGroupTypes(GroupTypeMask groupTypeMask)
{
    QList<Action::GroupType> groupTypes;

    if (groupTypeMask & ACTION_GROUP_APPLICATION) groupTypes.append(ACTION_GROUP_APPLICATION);
    if (groupTypeMask & ACTION_GROUP_FILE) groupTypes.append(ACTION_GROUP_FILE);
    if (groupTypeMask & ACTION_GROUP_VIEW) groupTypes.append(ACTION_GROUP_VIEW);
    if (groupTypeMask & ACTION_GROUP_EDIT) groupTypes.append(ACTION_GROUP_EDIT);
    if (groupTypeMask & ACTION_GROUP_DEBUG) groupTypes.append(ACTION_GROUP_DEBUG);
    if (groupTypeMask & ACTION_GROUP_CLOUD) groupTypes.append(ACTION_GROUP_CLOUD);

    return groupTypes;
}

QList<RAction::Definition> Action::generateActionDefinitionList()
{
    QList<RAction::Definition> actionDef;

    actionDef.resize(ACTION_N_TYPES);

    actionDef[ACTION_SEPARATOR] = RAction::definition();
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_SETTINGS, tr("Application settings"), "", "Ctrl+A", ":/icons/application/pixmaps/range-settings.svg", static_cast<PointerToMemberTrigger>(&Action::onApplicationSettings), QAction::PreferencesRole);
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_UPDATE, tr("Check for update"), "", "", ":/icons/application/pixmaps/range-update.svg", static_cast<PointerToMemberTrigger>(&Action::onUpdate));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_ABOUT, tr("About"), "", "F1", ":/icons/application/pixmaps/range-about.svg", static_cast<PointerToMemberTrigger>(&Action::onAbout), QAction::AboutRole);
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_ABOUT_QT, tr("About Qt"), "", "", ":/icons/application/pixmaps/range-qt.svg", static_cast<PointerToMemberTrigger>(&Action::onAboutQt), QAction::AboutQtRole);
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_LICENSE, tr("License"), "", "", ":/icons/application/pixmaps/range-license.svg", static_cast<PointerToMemberTrigger>(&Action::onLicense));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_RELEASE_NOTES, tr("Release notes"), "", "", ":/icons/application/pixmaps/range-release_notes.svg", static_cast<PointerToMemberTrigger>(&Action::onReleaseNotes));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_HELP, tr("Help"), "", "Ctrl+H", ":/icons/application/pixmaps/range-help.svg", static_cast<PointerToMemberTrigger>(&Action::onHelp));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_QUIT, tr("Quit"), "", "Q", ":/icons/application/pixmaps/range-quit.svg", static_cast<PointerToMemberTrigger>(&Action::onQuit), QAction::QuitRole);
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_NEW, tr("New"), "", "Ctrl+N", ":/icons/file/pixmaps/range-new.svg", static_cast<PointerToMemberTrigger>(&Action::onFileNew));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_OPEN, tr("Open"), "", "Ctrl+O", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFileOpen));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_IMPORT, tr("Import"), "", "Ctrl+I", ":/icons/file/pixmaps/range-import.svg", static_cast<PointerToMemberTrigger>(&Action::onFileImport));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_COMPARE, tr("Compare"), "", "Ctrl+D", ":/icons/file/pixmaps/range-compare.svg", static_cast<PointerToMemberTrigger>(&Action::onFileCompare));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_SAVE, tr("Save"), "", "Ctrl+S", ":/icons/file/pixmaps/range-save.svg", static_cast<PointerToMemberTrigger>(&Action::onFileSave));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_SAVE_AS, tr("Save as"), "", "Ctrl+Shift+S", ":/icons/file/pixmaps/range-save_as.svg", static_cast<PointerToMemberTrigger>(&Action::onFileSaveAs));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_CLOSE, tr("Close"), "", "Ctrl+W", ":/icons/file/pixmaps/range-close.svg", static_cast<PointerToMemberTrigger>(&Action::onFileClose));
    Action::regDef(actionDef, ACTION_GROUP_CLOUD, ACTION_CLOUD_SESSION_MANAGER, tr("Cloud session manager"), "", "", ":/icons/cloud/pixmaps/range-session_manager.svg", static_cast<PointerToMemberTrigger>(&Action::onCloudSessionManager));
    Action::regDef(actionDef, ACTION_GROUP_CLOUD, ACTION_CLOUD_FILE_MANAGER, tr("Cloud file manager"), "", "", ":/icons/cloud/pixmaps/range-file_manager.svg", static_cast<PointerToMemberTrigger>(&Action::onCloudFileManager));

    return actionDef;
}

void Action::regDef(QList<Definition> &actionDefinition, GroupType groupType, Type type, const QString &text, const QString &description, const QString &shortCut, const QString &icon, PointerToMemberTrigger triggerSlot, MenuRole menuRole)
{
    actionDefinition[type] = RAction::definition(Action::getGroupName(groupType),
                                                 Action::getName(type),
                                                 text,
                                                 description,
                                                 shortCut,
                                                 icon,
                                                 triggerSlot,
                                                 menuRole);
}

void Action::regDef(QList<Definition> &actionDefinition, GroupType groupType, Type type, const QString &text, const QString &description, const QString &shortCut, const QString &icon, PointerToMemberToggle toggleSlot, MenuRole menuRole)
{
    actionDefinition[type] = RAction::definition(Action::getGroupName(groupType),
                                                 Action::getName(type),
                                                 text,
                                                 description,
                                                 shortCut,
                                                 icon,
                                                 toggleSlot,
                                                 menuRole);
}

void Action::onFileNew()
{
    R_LOG_TRACE_IN;
//    NewModelDialog(Application::instance()->getMainWindow()).exec();
    R_LOG_TRACE_OUT;
}

void Action::onFileOpen()
{
    R_LOG_TRACE_IN;
    QString dialogDesc = "Family tree files (*.xml *.json);;Any files (*)";
    QString fileName = QFileDialog::getOpenFileName(Application::instance()->getMainWindow(),
                                                    tr("Open tree"),
                                                    Application::instance()->getApplicationSettings()->getDataDir(),
                                                    dialogDesc,
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);
    if (!fileName.isEmpty())
    {
        Application::instance()->getSession()->setTreeFileName(fileName);
        RToolInput toolInput;
        toolInput.addAction(FToolAction::readTreeFile(Application::instance()->getSession()->getTree(),fileName));
        this->executeToolAction(toolInput);
    }
    R_LOG_TRACE_OUT;
}

void Action::onFileImport()
{
    R_LOG_TRACE_IN;

    if (Application::instance()->getSession()->getTreeChanged())
    {
        QString question = tr("Current family tree contains unsaved changes.")
                         + "<br/>"
                         + tr("Would you like to save it first?");
        if (RMessageBox::question(Application::instance()->getMainWindow(),tr("Save current tree"),question) == RMessageBox::Yes)
        {
            this->onFileSave();
        }
    }

    QString dialogDesc = "Family tree files (*.xml *.json);;Any files (*)";
    QString fileName = QFileDialog::getOpenFileName(Application::instance()->getMainWindow(),
                                                    tr("Import tree"),
                                                    Application::instance()->getApplicationSettings()->getDataDir(),
                                                    dialogDesc,
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);
    if (!fileName.isEmpty())
    {
        TreeDiffDialog *dialog = new TreeDiffDialog(fileName,Application::instance()->getSession()->getTreeFileName(),true,Application::instance()->getMainWindow());
        dialog->exec();

    }
    R_LOG_TRACE_OUT;
}

void Action::onFileCompare()
{
    R_LOG_TRACE_IN;

    TreeDiffDialog *dialog = new TreeDiffDialog(QString(),Application::instance()->getSession()->getTreeFileName(),false,Application::instance()->getMainWindow());
    dialog->exec();

    R_LOG_TRACE_OUT;
}

void Action::onFileSave()
{
    R_LOG_TRACE_IN;

    QString fileName = Application::instance()->getSession()->getTreeFileName();

    if (fileName.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(Application::instance()->getMainWindow(),
                                                "Save tree as",
                                                Application::instance()->getApplicationSettings()->getDataDir() + "/tree.xml",
                                                "Family tree files (*.xml *.json);;Any files (*)",
                                                nullptr,
                                                QFileDialog::DontUseNativeDialog);
    }

    if (!fileName.isEmpty())
    {
        RToolInput toolInput;
        toolInput.addAction(FToolAction::writeTreeFile(Application::instance()->getSession()->getTree(),fileName));
        this->executeToolAction(toolInput);
    }
    R_LOG_TRACE_OUT;
}

void Action::onFileSaveAs()
{
    R_LOG_TRACE_IN;

    QString fileName = Application::instance()->getSession()->getTreeFileName();

    if (fileName.isEmpty())
    {
        fileName = Application::instance()->getApplicationSettings()->getDataDir() + "/tree.xml";
    }

    fileName = QFileDialog::getSaveFileName(Application::instance()->getMainWindow(),
                                            "Save tree as",
                                            fileName,
                                            "Family tree files (*.xml *.json);;Any files (*)",
                                            nullptr,
                                            QFileDialog::DontUseNativeDialog);

    if (!fileName.isEmpty())
    {
        RToolInput toolInput;
        toolInput.addAction(FToolAction::writeTreeFile(Application::instance()->getSession()->getTree(),fileName));
        this->executeToolAction(toolInput);
    }
    R_LOG_TRACE_OUT;
}

void Action::onFileClose()
{
    R_LOG_TRACE_IN;
    int response = RMessageBox::question(Application::instance()->getMainWindow(),
                                         tr("Close tree?"),
                                         tr("Are you sure you want to close current family tree?"));
    if (response == RMessageBox::Yes)
    {
        RLogger::info("Closing active tree.\n");
        Application::instance()->getSession()->getTree()->clear();
        Application::instance()->getSession()->setTreeFileName(QString());
    }
    R_LOG_TRACE_OUT;
}

void Action::onHelp()
{
    R_LOG_TRACE_IN;

    QMap<QString,QString> topics;

    topics.insert("general",tr("General"));
    topics.insert("tutorials",tr("Tutorials"));

    RHelpDialog *helpDialog = new RHelpDialog(tr("Help"),topics,Application::instance()->getMainWindow());
    helpDialog->setModal(false);
    helpDialog->show();

    R_LOG_TRACE_OUT;
}

void Action::onQuit()
{
    R_LOG_TRACE_IN;
    Application::instance()->getMainWindow()->close();
    R_LOG_TRACE_OUT;
}

void Action::onApplicationSettings()
{
    R_LOG_TRACE_IN;
    RApplicationSettingsDialog applicationSettingsDialog(Application::instance()->getApplicationSettings(),Application::instance()->getMainWindow());
    applicationSettingsDialog.exec();
    R_LOG_TRACE_OUT;
}

void Action::onUpdate()
{
    R_LOG_TRACE_IN;
    RSoftwareManagerDialog softwareManagerDialog(Application::instance()->getApplicationSettings(),Application::instance()->getMainWindow());
    softwareManagerDialog.exec();
    R_LOG_TRACE_OUT;
}

void Action::onAbout()
{
    R_LOG_TRACE_IN;
    QString title = tr("About");
    QString body = QString("<h1>%1</h1>").arg(RVendor::name())
                 + QString("<h2>%1</h2>").arg(RVendor::description())
                 + QString("%1<br/>").arg(RVendor::version().toString())
                 + QString("%1 &copy; %2<br/>").arg(RVendor::author().toHtmlEscaped(),QString::number(RVendor::year()))
                 + QString("<a href=\"mailto:%1\">%1</a><br/>").arg(RVendor::email())
                 + QString("<a href=\"%1\">%1</a>").arg(RVendor::www());

    QMessageBox::about(Application::instance()->getMainWindow(),title,body);
    R_LOG_TRACE_OUT;
}

void Action::onAboutQt()
{
    R_LOG_TRACE_IN;
    QMessageBox::aboutQt(Application::instance()->getMainWindow(),tr("About Qt"));
    R_LOG_TRACE_OUT;
}

void Action::onLicense()
{
    R_LOG_TRACE_IN;
    QString licenseFileName(Application::instance()->getApplicationSettings()->findLicenseFileName());

    try
    {
        QFile file(licenseFileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \'%s\'.",licenseFileName.toUtf8().constData());
        }
        QString licenseText(file.readAll());
        file.close();

        RTextBrowserDialog textBrowserDialog(tr("License"),
                                            licenseFileName,
                                            licenseText,
                                            Application::instance()->getMainWindow());
        textBrowserDialog.exec();
    }
    catch (const RError &RError)
    {
        RLogger::error("Failed to display license from file \'%s\'. %s\n",licenseFileName.toUtf8().constData(),RError.getMessage().toUtf8().constData());
    }
    catch (...)
    {
        RLogger::error("Failed to display license from file \'%s\'.\n",licenseFileName.toUtf8().constData());
    }
    R_LOG_TRACE_OUT;
}

void Action::onReleaseNotes()
{
    R_LOG_TRACE_IN;
    QString releaseNotesFileName(Application::instance()->getApplicationSettings()->findReleaseNotesFileName());

    try
    {
        QFile file(releaseNotesFileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \'%s\'.",releaseNotesFileName.toUtf8().constData());
        }
        QString releaseNotesText(file.readAll());
        file.close();

        RTextBrowserDialog textBrowserDialog(tr("Release notes"), releaseNotesFileName, releaseNotesText, Application::instance()->getMainWindow());
        textBrowserDialog.exec();
    }
    catch (const RError &RError)
    {
        RLogger::error("Failed to display release notes from file \'%s\'. %s\n",releaseNotesFileName.toUtf8().constData(),RError.getMessage().toUtf8().constData());
    }
    catch (...)
    {
        RLogger::error("Failed to display release notes from file \'%s\'.\n",releaseNotesFileName.toUtf8().constData());
    }
    R_LOG_TRACE_OUT;
}

void Action::onCloudSessionManager()
{
    R_LOG_TRACE_IN;
    RCloudSessionDialog dialog(Application::instance()->getCloudSessionManager()->findSession(Application::instance()->getCloudSessionManager()->findActiveSessionName()),
                               Application::instance()->getCloudConnectionHandler(),
                               Application::instance()->getApplicationSettings(),
                               Application::instance()->getMainWindow());
    if (dialog.exec() == QDialog::Accepted)
    {
        Application::instance()->getCloudSessionManager()->insertSession(dialog.getSessionInfo());
    }
    R_LOG_TRACE_OUT;
}

void Action::onCloudFileManager()
{
    R_LOG_TRACE_IN;
    RCloudFileManagerDialog cloudFileManagerDialog(Application::instance()->getCloudConnectionHandler(),
                                                   Application::instance()->getApplicationSettings(),
                                                   false,
                                                   Application::instance()->getMainWindow());
    cloudFileManagerDialog.exec();
    R_LOG_TRACE_OUT;
}
