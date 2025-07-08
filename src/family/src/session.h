#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>

#include <rfl_tree.h>

class Session : public QObject
{

    Q_OBJECT

    protected:

        //! Family tree
        FTree *pTree;
        //! Tree file name
        QString treeFileName;
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

        //! Return tree changed indicator value.
        bool getTreeChanged() const;

    protected slots:

        //! Handle tree changed signal.
        void onTreeChanged();

        //! Handle tree saved signal.
        void onTreeFileSaved(const QString &fileName);

        //! Handle tree file has changed.
        void onTreeFileChanged(const QString &fileName);

    signals:

        //! Handle tree file has changed.
        void treeFileChanged(const QString &fileName);

};

#endif /* SESSION_H */
