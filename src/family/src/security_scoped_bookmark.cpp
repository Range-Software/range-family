#include "security_scoped_bookmark.h"

#include <rbl_logger.h>

#ifdef Q_OS_DARWIN

#include <CoreFoundation/CoreFoundation.h>

//! URL of the file the last resolved bookmark refers to (access started).
static CFURLRef accessedUrl = nullptr;

static CFURLRef createFileUrl(const QString &fileName)
{
    QByteArray path = fileName.toUtf8();
    return CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                   reinterpret_cast<const UInt8*>(path.constData()),
                                                   CFIndex(path.size()),
                                                   false);
}

static QString urlToFileName(CFURLRef url)
{
    char buffer[4096];
    if (!CFURLGetFileSystemRepresentation(url,true,reinterpret_cast<UInt8*>(buffer),sizeof(buffer)))
    {
        return QString();
    }
    return QString::fromUtf8(buffer);
}

QByteArray SecurityScopedBookmark::create(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return QByteArray();
    }

    CFURLRef url = createFileUrl(fileName);
    if (!url)
    {
        return QByteArray();
    }

    CFErrorRef error = nullptr;
    CFDataRef data = CFURLCreateBookmarkData(kCFAllocatorDefault,
                                             url,
                                             kCFURLBookmarkCreationWithSecurityScope,
                                             nullptr,
                                             nullptr,
                                             &error);
    CFRelease(url);

    if (!data)
    {
        RLogger::warning("Failed to create security-scoped bookmark for file \"%s\".\n",fileName.toUtf8().constData());
        if (error)
        {
            CFRelease(error);
        }
        return QByteArray();
    }

    QByteArray bookmark(reinterpret_cast<const char*>(CFDataGetBytePtr(data)),int(CFDataGetLength(data)));
    CFRelease(data);
    return bookmark;
}

QString SecurityScopedBookmark::resolve(const QByteArray &bookmark)
{
    if (bookmark.isEmpty())
    {
        return QString();
    }

    CFDataRef data = CFDataCreate(kCFAllocatorDefault,
                                  reinterpret_cast<const UInt8*>(bookmark.constData()),
                                  CFIndex(bookmark.size()));
    if (!data)
    {
        return QString();
    }

    Boolean isStale = false;
    CFErrorRef error = nullptr;
    CFURLRef url = CFURLCreateByResolvingBookmarkData(kCFAllocatorDefault,
                                                      data,
                                                      kCFURLBookmarkResolutionWithSecurityScope | kCFURLBookmarkResolutionWithoutUIMask,
                                                      nullptr,
                                                      nullptr,
                                                      &isStale,
                                                      &error);
    CFRelease(data);

    if (!url)
    {
        RLogger::warning("Failed to resolve security-scoped bookmark.\n");
        if (error)
        {
            CFRelease(error);
        }
        return QString();
    }

    if (!CFURLStartAccessingSecurityScopedResource(url))
    {
        RLogger::warning("Failed to access security-scoped resource.\n");
        CFRelease(url);
        return QString();
    }

    // Stale bookmark still resolves - it is renewed when the settings are written.
    SecurityScopedBookmark::stopAccessing();
    accessedUrl = url;

    return urlToFileName(url);
}

void SecurityScopedBookmark::stopAccessing()
{
    if (accessedUrl)
    {
        CFURLStopAccessingSecurityScopedResource(accessedUrl);
        CFRelease(accessedUrl);
        accessedUrl = nullptr;
    }
}

#else

QByteArray SecurityScopedBookmark::create(const QString &)
{
    return QByteArray();
}

QString SecurityScopedBookmark::resolve(const QByteArray &)
{
    return QString();
}

void SecurityScopedBookmark::stopAccessing()
{
}

#endif
