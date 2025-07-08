#ifndef APPLICATION_H
#define APPLICATION_H

#include <rgl_application.h>
#include <rgl_application_settings.h>

#include "session.h"

class Application : public RApplication
{

    Q_OBJECT

    protected:

        //! Session.
        Session *session;

    public:

        //! Constructor.
        explicit Application(int &argc, char **argv);

        //! Return application instance.
        static Application *instance() noexcept;

        //! Return const reference to session.
        const Session *getSession() const;

        //! Return reference to session.
        Session *getSession();

        //! Initialize main window and other user objects.
        void initialize();

        //! Finalize main window and other user objects.
        void finalize();

    private slots:

        //! Handle tree file has changed.
        void onTreeFileChanged(const QString &fileName);

};

#endif // APPLICATION_H
