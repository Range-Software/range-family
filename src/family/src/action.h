#ifndef ACTION_H
#define ACTION_H

#include <QAction>
#include <QPointer>

#include <rcl_cloud_session_manager.h>

#include <rgl_action.h>

class CloudAiQueryDialog;

#define ACTION_GROUP_ALL (             \
    Action::ACTION_GROUP_APPLICATION | \
    Action::ACTION_GROUP_FILE        | \
    Action::ACTION_GROUP_VIEW        | \
    Action::ACTION_GROUP_EDIT        | \
    Action::ACTION_GROUP_DEBUG       | \
    Action::ACTION_GROUP_CLOUD       | \
    Action::ACTION_GROUP_AI            \
    )

class Action : public RAction
{

    Q_OBJECT

    public:

        typedef int GroupTypeMask;

        enum GroupType
        {
            ACTION_GROUP_NONE        = 0,
            ACTION_GROUP_APPLICATION = 1 << 1,
            ACTION_GROUP_FILE        = 1 << 2,
            ACTION_GROUP_VIEW        = 1 << 3,
            ACTION_GROUP_EDIT        = 1 << 4,
            ACTION_GROUP_DEBUG       = 1 << 5,
            ACTION_GROUP_CLOUD       = 1 << 6,
            ACTION_GROUP_AI          = 1 << 7
        };

        enum Type
        {
            ACTION_SEPARATOR = 0,
            ACTION_APPLICATION_SETTINGS,
            ACTION_APPLICATION_RIBBON_LAYOUT,
            ACTION_APPLICATION_RIBBON_COMPACT,
            ACTION_APPLICATION_UPDATE,
            ACTION_APPLICATION_ABOUT,
            ACTION_APPLICATION_ABOUT_QT,
            ACTION_APPLICATION_LICENSE,
            ACTION_APPLICATION_PRIVACY_POLICY,
            ACTION_APPLICATION_RELEASE_NOTES,
            ACTION_APPLICATION_HELP,
            ACTION_APPLICATION_FOLDER_CERT,
            ACTION_APPLICATION_FOLDER_DATA,
            ACTION_APPLICATION_FOLDER_DOC,
            ACTION_APPLICATION_FOLDER_LOG,
            ACTION_APPLICATION_FOLDER_TMP,
            ACTION_APPLICATION_QUIT,
            ACTION_FILE_NEW,
            ACTION_FILE_OPEN,
            ACTION_FILE_IMPORT,
            ACTION_FILE_COMPARE,
            ACTION_FILE_SAVE,
            ACTION_FILE_SAVE_AS,
            ACTION_FILE_CLOSE,
            ACTION_FILE_STORE_IMAGES_IN_TREE,
            ACTION_FILE_STORE_IMAGES_IN_DATA_DIR,
            ACTION_VIEW_PERSON_DETAILS,
            ACTION_VIEW_RELATIONS,
            ACTION_VIEW_FAMILY_TREE,
            ACTION_VIEW_OUTPUT,
            ACTION_EDIT_PERSON_ADD,
            ACTION_EDIT_PERSON_EDIT,
            ACTION_EDIT_PERSON_REMOVE,
            ACTION_EDIT_PERSON_SHOW_TREE,
            ACTION_EDIT_PERSON_SET_ACTIVE,
            ACTION_EDIT_PERSON_MERGE,
            ACTION_EDIT_PERSON_SUGGEST_DATES,
            ACTION_EDIT_RELATION_ADD,
            ACTION_EDIT_RELATION_EDIT,
            ACTION_EDIT_RELATION_REMOVE,
            ACTION_EDIT_RELATION_SHOW_TREE,
            ACTION_CLOUD_SESSION_MANAGER,
            ACTION_CLOUD_FILE_MANAGER,
            ACTION_CLOUD_AI_QUERY,
            ACTION_AI_SETTINGS_MANAGER,
            ACTION_AI_CHAT,
            ACTION_N_TYPES
        };

    private:

        //! Cloud AI query dialog. Only one such dialog can be opened at a time.
        static QPointer<CloudAiQueryDialog> cloudAiQueryDialog;

        //! Constructor.
        explicit Action(Definition definition, QObject *parent = nullptr);

    public:

        //! Return action group name.
        static QString getGroupName(GroupType groupType);

        //! Return action name.
        static QString getName(Type type);

        //! Return list of group types.
        static QList<GroupType> getGroupTypes(GroupTypeMask groupTypeMask = ACTION_GROUP_ALL);

        //! Generate list action descriptions.
        static QList<RAction::Definition> generateActionDefinitionList();

    private:

        static void regDef(QList<RAction::Definition> &actionDefinition,
                           GroupType groupType,
                           Type type,
                           const QString &text,
                           const QString &description,
                           const QString &shortCut,
                           const QString &icon,
                           PointerToMemberTrigger triggerSlot,
                           QAction::MenuRole menuRole = QAction::NoRole);

        static void regDef(QList<RAction::Definition> &actionDefinition,
                           GroupType groupType,
                           Type type,
                           const QString &text,
                           const QString &description,
                           const QString &shortCut,
                           const QString &icon,
                           PointerToMemberToggle toggleSlot,
                           QAction::MenuRole menuRole = QAction::NoRole);

        //! Open given folder in the platform file browser.
        static void openFolder(const QString &dirPath);

    public slots:

        //! New file.
        void onFileNew();

        //! Open file.
        void onFileOpen();

        //! Import file.
        void onFileImport();

        //! Compare files.
        void onFileCompare();

        //! Save file.
        void onFileSave();

        //! Save file as ...
        void onFileSaveAs();

        //! Close file.
        void onFileClose();

        //! Move all images stored in the data directory into the family tree.
        void onFileStoreImagesInTree();

        //! Move all images stored in the family tree into the data directory.
        void onFileStoreImagesInDataDir();

        //! Add new person.
        void onEditPersonAdd();

        //! Edit selected persons.
        void onEditPersonEdit();

        //! Remove selected persons.
        void onEditPersonRemove();

        //! Show family tree diagram of selected persons.
        void onEditPersonShowTree();

        //! Set selected person as active.
        void onEditPersonSetActive();

        //! Merge selected persons.
        void onEditPersonMerge();

        //! Suggest birth and death dates of selected persons.
        void onEditPersonSuggestDates();

        //! Add new relation.
        void onEditRelationAdd();

        //! Edit selected relations.
        void onEditRelationEdit();

        //! Remove selected relations.
        void onEditRelationRemove();

        //! Show family tree diagram of selected relations.
        void onEditRelationShowTree();

        //! Help center.
        void onHelp();

        //! Quit program.
        void onQuit();

        //! Application settings.
        void onApplicationSettings();

        //! Switch between ribbon layout and layout with menus and tool bars.
        void onApplicationRibbonLayout(bool checked);

        //! Switch between compact and regular ribbon.
        void onApplicationRibbonCompact(bool checked);

        //! Check for update.
        void onUpdate();

        //! About.
        void onAbout();

        //! About Qt.
        void onAboutQt();

        //! License.
        void onLicense();

        //! Privacy policy.
        void onPrivacyPolicy();

        //! Release notes.
        void onReleaseNotes();

        //! Open certificates folder.
        void onFolderCert();

        //! Open data folder.
        void onFolderData();

        //! Open documents folder.
        void onFolderDoc();

        //! Open log folder.
        void onFolderLog();

        //! Open temporary files folder.
        void onFolderTmp();

        //! Open cloud session manager.
        void onCloudSessionManager();

        //! Cloud file manager.
        void onCloudFileManager();

        //! Cloud query.
        void onCloudAiQuery();

        //! AI settings manager.
        void onAiSettingsManager();

        //! AI chat.
        void onAiChat(bool checked);

        //! Show or hide person details.
        void onViewPersonDetails(bool checked);

        //! Show or hide list of relations.
        void onViewRelations(bool checked);

        //! Show or hide family tree diagram.
        void onViewFamilyTree(bool checked);

        //! Show or hide application output.
        void onViewOutput(bool checked);

};

#endif // ACTION_H
