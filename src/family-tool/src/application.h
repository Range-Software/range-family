#ifndef APPLICATION_H
#define APPLICATION_H

#include <rfl_person.h>
#include <rfl_tree.h>

#include <QCoreApplication>

class Application : public QCoreApplication
{

    Q_OBJECT

    protected:

        //! Family tree.
        FTree *pTree;

    public:

        //! Constructor.
        explicit Application(int &argc, char **argv);

        //! Get family tree.
        FTree *getTree();

    signals:

        //! Application has started.
        void started(void);

    protected slots:

        //! Catch started signal.
        void onStarted(void);

};

#endif // APPLICATION_H
