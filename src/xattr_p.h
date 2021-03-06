/*
 * This file is part of the KDE Baloo Project
 * Copyright (C) 2014 Raphael Kubo da Costa <rakuco@FreeBSD.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef KFILEMETADATA_XATTR_P_H
#define KFILEMETADATA_XATTR_P_H

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#if defined(Q_OS_LINUX) || defined(__GLIBC__)
#include <sys/types.h>
#include <sys/xattr.h>
#include <attr/xattr.h>
#include <errno.h>
#elif defined(Q_OS_MAC)
#include <sys/types.h>
#include <sys/xattr.h>
#include <errno.h>
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
#include <sys/types.h>
#include <sys/extattr.h>
#include <errno.h>
#elif defined(Q_OS_WIN)
#include <windows.h>
#define ssize_t SSIZE_T
#endif

inline ssize_t k_getxattr(const QString& path, const QString& name, QString* value)
{
    const QByteArray p = QFile::encodeName(path);
    const char* encodedPath = p.constData();

    const QByteArray n = name.toUtf8();
    const char* attributeName = n.constData();

    // First get the size of the data we are going to get to reserve the right amount of space.
#if defined(Q_OS_LINUX) || (defined(__GLIBC__) && !defined(__stub_getxattr))
    const ssize_t size = getxattr(encodedPath, attributeName, NULL, 0);
#elif defined(Q_OS_MAC)
    const ssize_t size = getxattr(encodedPath, attributeName, NULL, 0, 0, 0);
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
    const ssize_t size = extattr_get_file(encodedPath, EXTATTR_NAMESPACE_USER, attributeName, NULL, 0);
#elif defined(Q_OS_WIN)
    const QString fullADSName = path + QLatin1Char(':') + name;
    HANDLE hFile = ::CreateFileW(reinterpret_cast<const WCHAR*>(fullADSName.utf16()), GENERIC_READ, FILE_SHARE_READ, NULL,
             OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if(!hFile) return 0;

    LARGE_INTEGER lsize;
    lsize.LowPart = GetFileSize(hFile, (DWORD*)&lsize.HighPart);
    ssize_t size = (qint64)lsize.QuadPart;
#else
    const ssize_t size = 0;
#endif

    if (size <= 0) {
        if (value) {
            value->clear();
        }
        return size;
    }

    QByteArray data(size, Qt::Uninitialized);

#if defined(Q_OS_LINUX) || (defined(__GLIBC__) && !defined(__stub_getxattr))
    const ssize_t r = getxattr(encodedPath, attributeName, data.data(), size);
#elif defined(Q_OS_MAC)
    const ssize_t r = getxattr(encodedPath, attributeName, data.data(), size, 0, 0);
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
    const ssize_t r = extattr_get_file(encodedPath, EXTATTR_NAMESPACE_USER, attributeName, data.data(), size);
#elif defined(Q_OS_WIN)
    ssize_t r = 0;
    // should we care about attributes longer than 2GiB? - unix xattr are restricted to much lower values
    ::ReadFile(hFile, data.data(), size, (DWORD*)&r, NULL);
    CloseHandle(hFile);
#else
    const ssize_t r = 0;
#endif

    if(value) *value = QString::fromUtf8(data);
    return r;
}

inline int k_setxattr(const QString& path, const QString& name, const QString& value)
{
    const QByteArray p = QFile::encodeName(path);
    const char* encodedPath = p.constData();

    const QByteArray n = name.toUtf8();
    const char* attributeName = n.constData();

    const QByteArray v = value.toUtf8();
    const void* attributeValue = v.constData();

    const size_t valueSize = v.size();

#if defined(Q_OS_LINUX) || (defined(__GLIBC__) && !defined(__stub_setxattr))
    return setxattr(encodedPath, attributeName, attributeValue, valueSize, 0);
#elif defined(Q_OS_MAC)
    return setxattr(encodedPath, attributeName, attributeValue, valueSize, 0, 0);
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
    const ssize_t count = extattr_set_file(encodedPath, EXTATTR_NAMESPACE_USER, attributeName, attributeValue, valueSize);
    return count == -1 ? -1 : 0;
#elif defined(Q_OS_WIN)
    const QString fullADSName = path + QLatin1Char(':') + name;
    HANDLE hFile = ::CreateFileW(reinterpret_cast<const WCHAR*>(fullADSName.utf16()), GENERIC_WRITE, FILE_SHARE_READ, NULL,
             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if(!hFile) return -1;

    DWORD count = 0;

    if(!::WriteFile(hFile, attributeValue, valueSize, &count, NULL)) {
        DWORD dw = GetLastError();
        TCHAR msg[1024];
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &msg,
            0, NULL );
        qWarning() << "failed to write to ADS:" << msg;
    }

    CloseHandle(hFile);
    return count;
#else
    return -1;
#endif
}


inline int k_removexattr(const QString& path, const QString& name)
{
    const QByteArray p = QFile::encodeName(path);
    const char* encodedPath = p.constData();

    const QByteArray n = name.toUtf8();
    const char* attributeName = n.constData();

    #if defined(Q_OS_LINUX) || (defined(__GLIBC__) && !defined(__stub_removexattr))
        return removexattr(encodedPath, attributeName);
    #elif defined(Q_OS_MAC)
        return removexattr(encodedPath, attributeName, XATTR_NOFOLLOW );
    #elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
        return extattr_delete_file (encodedPath, EXTATTR_NAMESPACE_USER, attributeName);
    #elif defined(Q_OS_WIN)
        const QString fullADSName = path + QLatin1Char(':') + name;
        int ret = (DeleteFileW(reinterpret_cast<const WCHAR*>(fullADSName.utf16()))) ? 0 : -1;
        return ret;
    #else
        return -1;
    #endif

}

inline bool k_hasAttribute(const QString& path, const QString& name)
{
#if defined(Q_OS_WIN)
    // enumerate all streams:
    const QString streamName = QStringLiteral(":") + name + QStringLiteral(":$DATA");
    HANDLE hFile = ::CreateFileW(reinterpret_cast<const WCHAR*>(path.utf16()), GENERIC_READ, FILE_SHARE_READ, NULL,
             OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if(!hFile) {
        return false;
    }
    FILE_STREAM_INFO* fi = new FILE_STREAM_INFO[256];
    if(GetFileInformationByHandleEx(hFile, FileStreamInfo, fi, 256 * sizeof(FILE_STREAM_INFO))) {
        if(QString::fromUtf16((ushort*)fi->StreamName, fi->StreamNameLength / sizeof(ushort)) == streamName) {
            delete[] fi;
            CloseHandle(hFile);
            return true;
        }
        FILE_STREAM_INFO* p = fi;
        do {
            p = (FILE_STREAM_INFO*) ((char*)p + p->NextEntryOffset);
            if(QString::fromUtf16((ushort*)p->StreamName, p->StreamNameLength / sizeof(ushort)) == streamName) {
                delete[] fi;
                CloseHandle(hFile);
                return true;
            }
        } while(p->NextEntryOffset != NULL);
    }
    delete[] fi;
    CloseHandle(hFile);
    return false;
#else
    k_getxattr(path, name, 0);
    return (errno != ENOATTR);
#endif
}

inline bool k_isSupported(const QString& path)
{
    QString value;
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
    k_getxattr(path, QStringLiteral("user.test"), &value);
    return (errno != ENOTSUP);
#elif defined(Q_OS_WIN)
    QFileInfo f(path);
    const QString drive = QString(f.absolutePath().left(2)) + QStringLiteral("\\");
    WCHAR szFSName[MAX_PATH];
    DWORD dwVolFlags;
    ::GetVolumeInformationW(reinterpret_cast<const WCHAR*>(drive.utf16()), NULL, 0, NULL, NULL, &dwVolFlags, szFSName, MAX_PATH);
    return ((dwVolFlags & FILE_NAMED_STREAMS) && _wcsicmp(szFSName, L"NTFS") == 0);
#else
    return false;
#endif
}
#endif // KFILEMETADATA_XATTR_P_H
