#include <QString>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <rbl_error.h>
#include <rbl_logger.h>
#include <rbl_utils.h>

#include <rfl_tool_action.h>

#include <rgl_application_settings_dialog.h>
#include <rgl_ai_agent_settings_manager_dialog.h>
#include <rgl_cloud_file_manager_dialog.h>
#include <rgl_cloud_session_dialog.h>
#include <rgl_desktop_services.h>
#include <rgl_help_dialog.h>
#include <rgl_message_box.h>
#include <rgl_release_notes_dialog.h>
#include <rgl_software_manager_dialog.h>
#include <rgl_text_browser_dialog.h>

#include "application.h"
#include "action.h"
#include "ai_chat_dialog.h"
#include "cloud_ai_query_dialog.h"
#include "editor_widget.h"
#include "image_button.h"
#include "main_window.h"
#include "tree_diff_dialog.h"

QPointer<CloudAiQueryDialog> Action::cloudAiQueryDialog;

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
        case ACTION_GROUP_AI:
            return QObject::tr("AI");
        default:
            return QString();
    }
}

QString Action::getName(Type type)
{
    switch (type)
    {
        case ACTION_APPLICATION_SETTINGS:          return "application-settings";
        case ACTION_APPLICATION_RIBBON_LAYOUT:     return "application-ribbon_layout";
        case ACTION_APPLICATION_RIBBON_COMPACT:    return "application-ribbon_compact";
        case ACTION_APPLICATION_UPDATE:            return "application-update";
        case ACTION_APPLICATION_ABOUT:             return "application-about";
        case ACTION_APPLICATION_ABOUT_QT:          return "application-about_qt";
        case ACTION_APPLICATION_LICENSE:           return "application-license";
        case ACTION_APPLICATION_PRIVACY_POLICY:    return "application-privacy_policy";
        case ACTION_APPLICATION_RELEASE_NOTES:     return "application-release_notes";
        case ACTION_APPLICATION_HELP:              return "application-help";
        case ACTION_APPLICATION_FOLDER_CERT:       return "application-folder_cert";
        case ACTION_APPLICATION_FOLDER_DATA:       return "application-folder_data";
        case ACTION_APPLICATION_FOLDER_DOC:        return "application-folder_doc";
        case ACTION_APPLICATION_FOLDER_LOG:        return "application-folder_log";
        case ACTION_APPLICATION_FOLDER_TMP:        return "application-folder_tmp";
        case ACTION_APPLICATION_QUIT:              return "application-quit";
        case ACTION_FILE_NEW:                      return "file-new";
        case ACTION_FILE_OPEN:                     return "file-open";
        case ACTION_FILE_IMPORT:                   return "file-import";
        case ACTION_FILE_COMPARE:                  return "file-compare";
        case ACTION_FILE_SAVE:                     return "file-save";
        case ACTION_FILE_SAVE_AS:                  return "file-save_as";
        case ACTION_FILE_CLOSE:                    return "file-close";
        case ACTION_FILE_STORE_IMAGES_IN_TREE:     return "file-store_images_in_tree";
        case ACTION_FILE_STORE_IMAGES_IN_DATA_DIR: return "file-store_images_in_data_dir";
        case ACTION_EDIT_PERSON_ADD:               return "edit-person_add";
        case ACTION_EDIT_PERSON_EDIT:              return "edit-person_edit";
        case ACTION_EDIT_PERSON_REMOVE:            return "edit-person_remove";
        case ACTION_EDIT_PERSON_SHOW_TREE:         return "edit-person_show_tree";
        case ACTION_EDIT_PERSON_SET_ACTIVE:        return "edit-person_set_active";
        case ACTION_EDIT_PERSON_MERGE:             return "edit-person_merge";
        case ACTION_EDIT_PERSON_SUGGEST_DATES:     return "edit-person_suggest_dates";
        case ACTION_EDIT_RELATION_ADD:             return "edit-relation_add";
        case ACTION_EDIT_RELATION_EDIT:            return "edit-relation_edit";
        case ACTION_EDIT_RELATION_REMOVE:          return "edit-relation_remove";
        case ACTION_EDIT_RELATION_SHOW_TREE:       return "edit-relation_show_tree";
        case ACTION_CLOUD_SESSION_MANAGER:         return "cloud-session_manager";
        case ACTION_CLOUD_FILE_MANAGER:            return "cloud-file_manager";
        case ACTION_CLOUD_AI_QUERY:                   return "cloud-query";
        case ACTION_AI_SETTINGS_MANAGER:           return "ai-settings-manager";
        case ACTION_AI_CHAT:                       return "ai-chat";
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
    if (groupTypeMask & ACTION_GROUP_AI) groupTypes.append(ACTION_GROUP_AI);

    return groupTypes;
}

QList<RAction::Definition> Action::generateActionDefinitionList()
{
    QList<RAction::Definition> actionDef;

    actionDef.resize(ACTION_N_TYPES);

    actionDef[ACTION_SEPARATOR] = RAction::definition();
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_SETTINGS, tr("Application settings"), "", "Ctrl+A", ":/icons/application/pixmaps/range-settings.svg", static_cast<PointerToMemberTrigger>(&Action::onApplicationSettings), QAction::PreferencesRole);
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_RIBBON_LAYOUT, tr("Ribbon layout"), tr("Show commands in a ribbon instead of menus and tool bars."), "", ":/icons/application/pixmaps/range-ribbon_layout.svg", static_cast<PointerToMemberToggle>(&Action::onApplicationRibbonLayout));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_RIBBON_COMPACT, tr("Compact ribbon"), tr("Show the ribbon in less vertical space, with smaller buttons and no group titles."), "", ":/icons/application/pixmaps/range-ribbon_compact.svg", static_cast<PointerToMemberToggle>(&Action::onApplicationRibbonCompact));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_UPDATE, tr("Check for update"), "", "", ":/icons/application/pixmaps/range-update.svg", static_cast<PointerToMemberTrigger>(&Action::onUpdate));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_ABOUT, tr("About"), "", "F1", ":/icons/application/pixmaps/range-about.svg", static_cast<PointerToMemberTrigger>(&Action::onAbout), QAction::AboutRole);
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_ABOUT_QT, tr("About Qt"), "", "", ":/icons/application/pixmaps/range-qt.svg", static_cast<PointerToMemberTrigger>(&Action::onAboutQt), QAction::AboutQtRole);
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_LICENSE, tr("License"), "", "", ":/icons/application/pixmaps/range-license.svg", static_cast<PointerToMemberTrigger>(&Action::onLicense));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_PRIVACY_POLICY, tr("Privacy policy"), "", "", ":/icons/application/pixmaps/range-privacy_policy.svg", static_cast<PointerToMemberTrigger>(&Action::onPrivacyPolicy));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_RELEASE_NOTES, tr("Release notes"), "", "", ":/icons/application/pixmaps/range-release_notes.svg", static_cast<PointerToMemberTrigger>(&Action::onReleaseNotes));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_FOLDER_CERT, tr("Certificates"), tr("Open certificates folder in file browser."), "", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFolderCert));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_FOLDER_DATA, tr("Data"), tr("Open data folder in file browser."), "", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFolderData));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_FOLDER_DOC, tr("Documents"), tr("Open documents folder in file browser."), "", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFolderDoc));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_FOLDER_LOG, tr("Logs"), tr("Open log folder in file browser."), "", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFolderLog));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_FOLDER_TMP, tr("Temporary files"), tr("Open temporary files folder in file browser."), "", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFolderTmp));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_HELP, tr("Help"), "", "Ctrl+H", ":/icons/application/pixmaps/range-help.svg", static_cast<PointerToMemberTrigger>(&Action::onHelp));
    Action::regDef(actionDef, ACTION_GROUP_APPLICATION, ACTION_APPLICATION_QUIT, tr("Quit"), "", "Q", ":/icons/application/pixmaps/range-quit.svg", static_cast<PointerToMemberTrigger>(&Action::onQuit), QAction::QuitRole);
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_NEW, tr("New"), "", "Ctrl+N", ":/icons/file/pixmaps/range-new.svg", static_cast<PointerToMemberTrigger>(&Action::onFileNew));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_OPEN, tr("Open"), "", "Ctrl+O", ":/icons/file/pixmaps/range-open.svg", static_cast<PointerToMemberTrigger>(&Action::onFileOpen));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_IMPORT, tr("Import"), "", "Ctrl+I", ":/icons/file/pixmaps/range-import.svg", static_cast<PointerToMemberTrigger>(&Action::onFileImport));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_COMPARE, tr("Compare"), "", "Ctrl+D", ":/icons/file/pixmaps/range-compare.svg", static_cast<PointerToMemberTrigger>(&Action::onFileCompare));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_SAVE, tr("Save"), "", "Ctrl+S", ":/icons/file/pixmaps/range-save.svg", static_cast<PointerToMemberTrigger>(&Action::onFileSave));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_SAVE_AS, tr("Save as"), "", "Ctrl+Shift+S", ":/icons/file/pixmaps/range-save_as.svg", static_cast<PointerToMemberTrigger>(&Action::onFileSaveAs));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_CLOSE, tr("Close"), "", "Ctrl+W", ":/icons/file/pixmaps/range-close.svg", static_cast<PointerToMemberTrigger>(&Action::onFileClose));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_STORE_IMAGES_IN_TREE, tr("Store all images in family tree"), "", "", ":/icons/file/pixmaps/range-import.svg", static_cast<PointerToMemberTrigger>(&Action::onFileStoreImagesInTree));
    Action::regDef(actionDef, ACTION_GROUP_FILE, ACTION_FILE_STORE_IMAGES_IN_DATA_DIR, tr("Store all images in data directory"), "", "", ":/icons/file/pixmaps/range-export.svg", static_cast<PointerToMemberTrigger>(&Action::onFileStoreImagesInDataDir));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_ADD, tr("Add person"), tr("Add a new person to the family tree."), "", ":/icons/family/pixmaps/range-person_add.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonAdd));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_EDIT, tr("Edit selected persons"), tr("Edit persons selected in the list of persons."), "", ":/icons/family/pixmaps/range-person_edit.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonEdit));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_REMOVE, tr("Remove selected persons"), tr("Remove persons selected in the list of persons from the family tree."), "", ":/icons/family/pixmaps/range-person_remove.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonRemove));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_SHOW_TREE, tr("Show family tree of selected persons"), tr("Show a diagram of the family tree around each person selected in the list of persons."), "", ":/icons/family/pixmaps/range-family_tree.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonShowTree));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_SET_ACTIVE, tr("Set selected person as active"), tr("Set the person selected in the list of persons as the active person."), "", ":/icons/family/pixmaps/range-person_active.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonSetActive));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_MERGE, tr("Merge selected persons"), tr("Merge persons selected in the list of persons into one person."), "", ":/icons/family/pixmaps/range-persons_merge.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonMerge));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_PERSON_SUGGEST_DATES, tr("Suggest birth and death dates"), tr("Suggest birth and death dates of selected persons from dates of their relatives."), "", ":/icons/family/pixmaps/range-suggest_dates.svg", static_cast<PointerToMemberTrigger>(&Action::onEditPersonSuggestDates));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_RELATION_ADD, tr("Add relation"), tr("Add a new relation to the family tree."), "", ":/icons/family/pixmaps/range-relation_add.svg", static_cast<PointerToMemberTrigger>(&Action::onEditRelationAdd));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_RELATION_EDIT, tr("Edit selected relations"), tr("Edit relations selected in the list of relations."), "", ":/icons/family/pixmaps/range-relation_edit.svg", static_cast<PointerToMemberTrigger>(&Action::onEditRelationEdit));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_RELATION_REMOVE, tr("Remove selected relations"), tr("Remove relations selected in the list of relations from the family tree."), "", ":/icons/family/pixmaps/range-relation_remove.svg", static_cast<PointerToMemberTrigger>(&Action::onEditRelationRemove));
    Action::regDef(actionDef, ACTION_GROUP_EDIT, ACTION_EDIT_RELATION_SHOW_TREE, tr("Show family tree of selected relations"), tr("Show a diagram of the family tree around each relation selected in the list of relations."), "", ":/icons/family/pixmaps/range-family_tree.svg", static_cast<PointerToMemberTrigger>(&Action::onEditRelationShowTree));
    Action::regDef(actionDef, ACTION_GROUP_CLOUD, ACTION_CLOUD_SESSION_MANAGER, tr("Cloud session manager"), "", "", ":/icons/cloud/pixmaps/range-session_manager.svg", static_cast<PointerToMemberTrigger>(&Action::onCloudSessionManager));
    Action::regDef(actionDef, ACTION_GROUP_CLOUD, ACTION_CLOUD_FILE_MANAGER, tr("Cloud file manager"), "", "", ":/icons/cloud/pixmaps/range-file_manager.svg", static_cast<PointerToMemberTrigger>(&Action::onCloudFileManager));
    Action::regDef(actionDef, ACTION_GROUP_CLOUD, ACTION_CLOUD_AI_QUERY, tr("Cloud AI query"), "", "", ":/icons/cloud/pixmaps/range-ai_query.svg", static_cast<PointerToMemberTrigger>(&Action::onCloudAiQuery));
    Action::regDef(actionDef, ACTION_GROUP_AI, ACTION_AI_SETTINGS_MANAGER, tr("AI settings manager"), "", "", ":/icons/ai/pixmaps/range-ai_settings_manager.svg", static_cast<PointerToMemberTrigger>(&Action::onAiSettingsManager));
    Action::regDef(actionDef, ACTION_GROUP_AI, ACTION_AI_CHAT, tr("AI chat"), "", "", ":/icons/ai/pixmaps/range-ai_chat.svg", static_cast<PointerToMemberTrigger>(&Action::onAiChat));

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
        // RToolInput toolInput;
        // toolInput.addAction(FToolAction::readTreeFile(Application::instance()->getSession()->getTree(),fileName));
        // this->executeToolAction(toolInput);
        Application::instance()->getSession()->readTreeFile();
    }
    R_LOG_TRACE_OUT;
}

void Action::onFileImport()
{
    R_LOG_TRACE_IN;

    if (Application::instance()->getSession()->getTreeChanged())
    {
        QString question = tr("The current family tree contains unsaved changes.")
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
                                                "Save tree",
                                                Application::instance()->getApplicationSettings()->getDataDir() + "/tree.xml",
                                                "Family tree files (*.xml *.json);;Any files (*)",
                                                nullptr,
                                                QFileDialog::DontUseNativeDialog);
    }

    if (!fileName.isEmpty())
    {
        // RToolInput toolInput;
        // toolInput.addAction(FToolAction::writeTreeFile(Application::instance()->getSession()->getTree(),fileName));
        // this->executeToolAction(toolInput);
        Application::instance()->getSession()->setTreeFileName(fileName);
        Application::instance()->getSession()->writeTreeFile();
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
        // RToolInput toolInput;
        // toolInput.addAction(FToolAction::writeTreeFile(Application::instance()->getSession()->getTree(),fileName));
        // this->executeToolAction(toolInput);
        Application::instance()->getSession()->setTreeFileName(fileName);
        Application::instance()->getSession()->writeTreeFile();
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
        Application::instance()->getSession()->clearObsoletePictureFiles();
        Application::instance()->getSession()->setTreeFileName(QString());
    }
    R_LOG_TRACE_OUT;
}

void Action::onFileStoreImagesInTree()
{
    R_LOG_TRACE_IN;

    Session *session = Application::instance()->getSession();
    FTree *pTree = session->getTree();

    QString question = tr("All images which are stored in the data directory will be moved into the family tree.")
                     + "<br/>"
                     + tr("An image stored within the family tree is reduced to occupy a maximum of %1 MB.")
                           .arg(ImageButton::maxTreeDataSize / (1024 * 1024))
                     + "<br/>"
                     + tr("Would you like to continue?");

    if (RMessageBox::question(Application::instance()->getMainWindow(),tr("Store images in family tree"),question) != RMessageBox::Yes)
    {
        R_LOG_TRACE_OUT;
        return;
    }

    uint nMovedImages = 0;

    const QList<QUuid> personIds = pTree->getPersons();
    for (const QUuid &personId : personIds)
    {
        FPerson person = pTree->findPerson(personId);
        FPicture picture = person.getPicture();

        if (picture.getUrl().isEmpty() || picture.getData().isEmpty())
        {
            // The image is already stored within the tree.
            continue;
        }

        QByteArray imageData = ImageButton::reduceImageData(picture.getData(),ImageButton::maxTreeDataSize);
        if (imageData.isEmpty())
        {
            RLogger::warning("Failed to store image of person \"%s\" within the family tree.\n",
                             personId.toString(QUuid::WithoutBraces).toUtf8().constData());
            continue;
        }

        QString pictureFilePath = picture.getUrl();

        picture.setUrl(QString());
        picture.setData(imageData);
        person.setPicture(picture);
        pTree->setPerson(person);

        // The picture file is no longer referenced - it is removed once the tree has been saved.
        session->addObsoletePictureFile(pictureFilePath);

        nMovedImages++;
    }

    RLogger::info("%u images were moved into the family tree.\n",nMovedImages);

    RMessageBox::information(Application::instance()->getMainWindow(),
                             tr("Store images in family tree"),
                             tr("Number of images moved into the family tree: %1").arg(nMovedImages)
                             + "<br/>"
                             + tr("The family tree must be saved for the change to take effect."));

    R_LOG_TRACE_OUT;
}

void Action::onFileStoreImagesInDataDir()
{
    R_LOG_TRACE_IN;

    Session *session = Application::instance()->getSession();
    FTree *pTree = session->getTree();

    QString treeFileName = session->getTreeFileName();

    if (treeFileName.isEmpty())
    {
        RMessageBox::warning(Application::instance()->getMainWindow(),
                             tr("Store images in data directory"),
                             tr("The family tree must be saved to a file before images can be stored as separate files."));
        R_LOG_TRACE_OUT;
        return;
    }

    QString question = tr("All images which are stored in the family tree will be moved into the data directory \"%1\".")
                           .arg(QFileInfo(treeFileName).absolutePath())
                     + "<br/>"
                     + tr("Would you like to continue?");

    if (RMessageBox::question(Application::instance()->getMainWindow(),tr("Store images in data directory"),question) != RMessageBox::Yes)
    {
        R_LOG_TRACE_OUT;
        return;
    }

    uint nMovedImages = 0;

    const QList<QUuid> personIds = pTree->getPersons();
    for (const QUuid &personId : personIds)
    {
        FPerson person = pTree->findPerson(personId);
        FPicture picture = person.getPicture();

        if (!picture.getUrl().isEmpty() || picture.getData().isEmpty())
        {
            // The image is already stored in a separate file.
            continue;
        }

        QString pictureFilePath = FTree::pictureFilePath(treeFileName,personId);
        if (pictureFilePath.isEmpty())
        {
            RLogger::warning("Failed to store image of person \"%s\" in the data directory.\n",
                             personId.toString(QUuid::WithoutBraces).toUtf8().constData());
            continue;
        }

        picture.setUrl(pictureFilePath);
        person.setPicture(picture);
        pTree->setPerson(person);

        nMovedImages++;
    }

    RLogger::info("%u images were moved into the data directory.\n",nMovedImages);

    RMessageBox::information(Application::instance()->getMainWindow(),
                             tr("Store images in data directory"),
                             tr("Number of images moved into the data directory: %1").arg(nMovedImages)
                             + "<br/>"
                             + tr("The family tree must be saved for the change to take effect."));

    R_LOG_TRACE_OUT;
}

void Action::onEditPersonAdd()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->addNewPerson();
    R_LOG_TRACE_OUT;
}

void Action::onEditPersonEdit()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->editSelectedPersons();
    R_LOG_TRACE_OUT;
}

void Action::onEditPersonRemove()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->removeSelectedPersons();
    R_LOG_TRACE_OUT;
}

void Action::onEditPersonShowTree()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->showSelectedPersonsDiagram();
    R_LOG_TRACE_OUT;
}

void Action::onEditPersonSetActive()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->setSelectedPersonActive();
    R_LOG_TRACE_OUT;
}

void Action::onEditPersonMerge()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->mergeSelectedPersons();
    R_LOG_TRACE_OUT;
}

void Action::onEditPersonSuggestDates()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getPersonsList()->suggestSelectedPersonsDates();
    R_LOG_TRACE_OUT;
}

void Action::onEditRelationAdd()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getRelationsList()->addNewRelation();
    R_LOG_TRACE_OUT;
}

void Action::onEditRelationEdit()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getRelationsList()->editSelectedRelations();
    R_LOG_TRACE_OUT;
}

void Action::onEditRelationRemove()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getRelationsList()->removeSelectedRelations();
    R_LOG_TRACE_OUT;
}

void Action::onEditRelationShowTree()
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->getEditor()->getRelationsList()->showSelectedRelationsDiagram();
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

void Action::onApplicationRibbonLayout(bool checked)
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->setRibbonLayout(checked);
    R_LOG_TRACE_OUT;
}

void Action::onApplicationRibbonCompact(bool checked)
{
    R_LOG_TRACE_IN;
    qobject_cast<MainWindow*>(Application::instance()->getMainWindow())->setRibbonCompact(checked);
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

void Action::onPrivacyPolicy()
{
    R_LOG_TRACE_IN;
    QString privacyPolicyFileName(Application::instance()->getApplicationSettings()->findPrivacyPolicyFileName());

    try
    {
        QFile file(privacyPolicyFileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \'%s\'.",privacyPolicyFileName.toUtf8().constData());
        }
        QString privacyPolicyText(file.readAll());
        file.close();

        RTextBrowserDialog textBrowserDialog(tr("Privacy policy"),
                                            privacyPolicyFileName,
                                            privacyPolicyText,
                                            Application::instance()->getMainWindow());
        textBrowserDialog.exec();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to display privacy policy from file \'%s\'. %s\n",privacyPolicyFileName.toUtf8().constData(),rError.getMessage().toUtf8().constData());
    }
    catch (...)
    {
        RLogger::error("Failed to display privacy policy from file \'%s\'.\n",privacyPolicyFileName.toUtf8().constData());
    }
    R_LOG_TRACE_OUT;
}

void Action::onReleaseNotes()
{
    R_LOG_TRACE_IN;
    RReleaseNotesDialog releaseNotesDialog(Application::instance()->getMainWindow());
    releaseNotesDialog.exec();
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

void Action::onCloudAiQuery()
{
    R_LOG_TRACE_IN;
    if (Action::cloudAiQueryDialog.isNull())
    {
        Action::cloudAiQueryDialog = new CloudAiQueryDialog(Application::instance()->getCloudConnectionHandler(),
                                                            Application::instance()->getApplicationSettings(),
                                                            Application::instance()->getMainWindow());
        Action::cloudAiQueryDialog->show();
    }
    else
    {
        // Dialog is already opened. Bring it to the front instead of opening another one.
        Action::cloudAiQueryDialog->showNormal();
        Action::cloudAiQueryDialog->raise();
        Action::cloudAiQueryDialog->activateWindow();
    }
    R_LOG_TRACE_OUT;
}

void Action::onAiSettingsManager()
{
    R_LOG_TRACE_IN;
    RAiAgentSettingsManagerDialog aiAgentSettingsManagerDialog(Application::instance()->getAiAgentSettingsManager(),
                                                               Application::instance()->getMainWindow());
    aiAgentSettingsManagerDialog.exec();
    R_LOG_TRACE_OUT;
}

void Action::onAiChat()
{
    R_LOG_TRACE_IN;
    AiChatDialog *aiChatDialog = new AiChatDialog(Application::instance()->getAiAgentSettingsManager(),
                                                  Application::instance()->getApplicationSettings(),
                                                  Application::instance()->getMainWindow());
    aiChatDialog->show();
    R_LOG_TRACE_OUT;
}

void Action::openFolder(const QString &dirPath)
{
    if (!RDesktopServices::openDirectory(dirPath))
    {
        RMessageBox::warning(Application::instance()->getMainWindow(),
                             tr("Failed to open folder"),
                             tr("Failed to open folder") + ": <tt>" + dirPath + "</tt>");
    }
}

void Action::onFolderCert()
{
    R_LOG_TRACE_IN;
    Action::openFolder(RApplicationSettings::getCertDir());
    R_LOG_TRACE_OUT;
}

void Action::onFolderData()
{
    R_LOG_TRACE_IN;
    Action::openFolder(RApplicationSettings::getDataDir());
    R_LOG_TRACE_OUT;
}

void Action::onFolderDoc()
{
    R_LOG_TRACE_IN;
    Action::openFolder(RApplicationSettings::getDocDir());
    R_LOG_TRACE_OUT;
}

void Action::onFolderLog()
{
    R_LOG_TRACE_IN;
    Action::openFolder(RApplicationSettings::getLogDir());
    R_LOG_TRACE_OUT;
}

void Action::onFolderTmp()
{
    R_LOG_TRACE_IN;
    Action::openFolder(RApplicationSettings::getTmpDir());
    R_LOG_TRACE_OUT;
}
