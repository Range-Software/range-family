#ifndef SECURITY_SCOPED_BOOKMARK_H
#define SECURITY_SCOPED_BOOKMARK_H

#include <QByteArray>
#include <QString>

//! Persistent access to files outside of the application sandbox container.
//! A sandboxed macOS application may access the file the user selected in a
//! file dialog only until it quits. Security-scoped bookmark stored in the
//! settings lets it regain the access on the next launch.
//! On other platforms (and when not sandboxed) the functions do nothing.
class SecurityScopedBookmark
{

    public:

        //! Create bookmark for given file.
        //! Access to the file must be granted when the bookmark is created.
        //! Return empty array on failure.
        static QByteArray create(const QString &fileName);

        //! Resolve bookmark and start accessing the file it refers to.
        //! The access lasts until stopAccessing() is called or the application quits.
        //! Return empty string on failure.
        static QString resolve(const QByteArray &bookmark);

        //! Stop accessing the file the last resolved bookmark refers to.
        static void stopAccessing();

};

#endif // SECURITY_SCOPED_BOOKMARK_H
