/*
    A conduit between KFileMetaData's native plugin system and plugins written
    in other languages, installed as binaries on the user's system

    Copyright (C) 2015  Boudhayan Gupta <bgupta@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "externalextractor.h"
#include "propertyinfo.h"
#include "typeinfo.h"

#include <QDir>
#include <QProcess>
#include <QTextCodec>
#include <QProcessEnvironment>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>

using namespace KFileMetaData;

// static methods first

QStringList ExternalExtractor::findExtractors()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList filter({ QStringLiteral("kfilemetadata_extractor_*") });
    QSet<QString> extractors;

    QString customPaths = env.value(QStringLiteral("KFILEMETADATA_EXTRACTOR_PATH"));
    QString paths = env.value(QStringLiteral("PATH"));

#if defined(Q_OS_UNIX)
    QChar sepChar = QLatin1Char(':');
#elif defined(Q_OS_WIN)
    QChar sepChar = QLatin1Char(';');
#endif

    QStringList pathList;

    // we'll look in cwd first
    pathList.append(QDir::currentPath());

    // then the custom paths
    if (!customPaths.isEmpty()) {
        pathList.append(customPaths.split(sepChar, QString::SkipEmptyParts));
    }

    // and finally in PATH
    if (!paths.isEmpty()) {
        pathList.append(paths.split(sepChar, QString::SkipEmptyParts));
    }

    // loop over the paths, find all the extractors
    Q_FOREACH(auto path, pathList) {
        QDir dir(path);
        QStringList candidates = dir.entryList(filter, QDir::Files | QDir::Executable, QDir::Unsorted);
        Q_FOREACH(auto candidate, candidates) {
            extractors.insert(dir.absoluteFilePath(candidate));
        }
    }

    // done
    return extractors.toList();
}

// object methods

ExternalExtractor::ExternalExtractor(QObject *parent) :
    ExtractorPlugin(parent),
    m_extractors(QMultiHash<QString, QString>()),
    m_textCodec(QTextCodec::codecForLocale())
{
    // we try to find all kfilemetadata_extractor_* binaries in PATH, and run them
    // with the --mimetypes flag to see what mimetypes they support

    Q_FOREACH(auto extractor, findExtractors()) {
        QProcess process;
        process.start(extractor, QStringList({ QStringLiteral("--mimetypes") }), QIODevice::ReadOnly);
        process.waitForFinished();

        while (!process.atEnd()) {
            QByteArray data = process.readLine();
            QString mimeType = m_textCodec->toUnicode(data);
            mimeType = mimeType.trimmed();
            m_extractors.insert(mimeType, extractor);
        }
    }
}

QStringList ExternalExtractor::mimetypes() const
{
    return m_extractors.keys();
}

void ExternalExtractor::extract(ExtractionResult *result)
{
    QString extractor = m_extractors.value(result->inputMimetype());
    QStringList filename({ result->inputUrl() });
    QByteArray output;

    // we start by running the extractor on the file and capturing all the
    // output

    {
        QProcess extractorProcess;
        extractorProcess.start(extractor, filename, QIODevice::ReadOnly);
        extractorProcess.waitForFinished();

        if (extractorProcess.exitStatus()) {
            return;
        }
        output = extractorProcess.readAll();
    }

    // now we read in the output (which is a standard json format) into the
    // ExtractionResult

    QJsonDocument extractorData = QJsonDocument::fromJson(output);
    if (!extractorData.isObject()) {
        return;
    }
    QJsonObject rootObject = extractorData.object();

    Q_FOREACH(auto key, rootObject.keys()) {
        if (key == QStringLiteral("typeInfo")) {
            TypeInfo info = TypeInfo::fromName(rootObject.value(key).toString());
            result->addType(info.type());
            continue;
        }

        PropertyInfo info = PropertyInfo::fromName(key);
        if (info.name() != key) {
            continue;
        }
        result->add(info.property(), rootObject.value(key).toVariant());
    }
}
