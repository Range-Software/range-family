#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>

#include <rbl_tool_action.h>
#include <rfl_tree.h>

class Session : public QObject
{

    Q_OBJECT

    protected:

        //! Family tree
        FTree *pTree;
        //! Tree file name
        QString treeFileName;
        //! Active person ID.
        QUuid activePersonId;
        //! Last AI chat agent.
        QString lastAiChatAgent;
        //! Last AI chat language.
        QString lastAiChatLanguage;
        //! Tree changed indicator.
        bool treeChanged;

        QFileSystemWatcher *fileSystemWatcher;

    public:

        //! Constructor.
        explicit Session(QObject *parent = nullptr);

        //! Return const pointer to family tree object.
        const FTree *getTree() const;

        //! Return pointer to family tree object.
        FTree *getTree();

        //! Return const reference to tree file name.
        const QString &getTreeFileName() const;

        //! Set tree new file name.
        void setTreeFileName(const QString &treeFileName);

        //! Return const reference to active person name.
        const QUuid &getActivePersonId() const;

        //! Set active person name.
        void setActivePersonId(const QUuid &activePersonId);

        //! Return const reference to last AI chat agent.
        const QString &getLastAiChatAgent() const;

        //! Set last AI chat agent.
        void setLastAiChatAgent(const QString &lastAiAgent);

        //! Return const reference to last AI chat language.
        const QString &getLastAiChatLanguage() const;

        //! Set last AI chat language.
        void setLastAiChatLanguage(const QString &lastAiChatLanguage);

        //! Return tree changed indicator value.
        bool getTreeChanged() const;

        //! Read tree file.
        void readTreeFile();

        //! Write tree file.
        void writeTreeFile() const;

    protected slots:

        //! Handle tree changed signal.
        void onTreeChanged();

        //! Handle tree saved signal.
        void onTreeFileSaved(const QString &fileName);

        //! Handle tree file has changed.
        void onTreeFileChanged(const QString &fileName);

        //! Write action has finished.
        void writeActionFinished(const QSharedPointer<RToolAction> &action);

        //! Write action has failed.
        void writeActionFailed(const QSharedPointer<RToolAction> &action);


    signals:

        //! Tree file has changed.
        void treeFileChanged(const QString &fileName);

        //! Tree file has been removed.
        void treeFileRemoved(const QString &fileName);

        //! Active person ID has changed.
        void activePersonIdChanged(const QUuid &fileName);

};

#endif /* SESSION_H */
