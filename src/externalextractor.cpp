/*
 * This file is part of the KFileMetaData project
 * Copyright (C) 2016  Varun Joshi <varunj.1011@gmail.com>
 * Copyright (C) 2015  Boudhayan Gupta <bgupta@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "externalextractor.h"
#include "properties.h"
#include "propertyinfo.h"
#include "typeinfo.h"

using namespace KFileMetaData;

class ExternalExtractor::Private {
public:
    QString path;
    QStringList writeMimetypes;
    QString mainPath;
};


ExternalExtractor::ExternalExtractor(QObject* parent)
    : ExtractorPlugin(parent)
{
}

ExternalExtractor::ExternalExtractor(const QString& pluginPath)
    : ExtractorPlugin(new QObject()),
      d(new Private)
{
    d->path = pluginPath;

    QDir pluginDir(pluginPath);
    QStringList pluginDirContents = pluginDir.entryList();

    if (!pluginDirContents.contains(QStringLiteral("manifest.json"))) {
        qDebug() << "Path does not seem to contain a valid plugin";
        return;
    }

    QFile manifest(pluginDir.filePath(QStringLiteral("manifest.json")));
    manifest.open(QIODevice::ReadOnly);
    QJsonDocument manifestDoc = QJsonDocument::fromJson(manifest.readAll());
    if (!manifestDoc.isObject()) {
        qDebug() << "Manifest does not seem to be a valid JSON Object";
        return;
    }

    QJsonObject rootObject = manifestDoc.object();
    QJsonArray mimetypesArray = rootObject.value(QStringLiteral("mimetypes")).toArray();
    QStringList mimetypes;
    Q_FOREACH(QVariant mimetype, mimetypesArray) {
        mimetypes << mimetype.toString();
    }

    d->writeMimetypes.append(mimetypes);
    d->mainPath = pluginDir.filePath(rootObject[QStringLiteral("main")].toString());
}

QStringList ExternalExtractor::mimetypes() const
{
    return d->writeMimetypes;
}

void ExternalExtractor::extract(ExtractionResult* result)
{
    QJsonDocument writeData;
    QJsonObject writeRootObject;
    QByteArray output;

    writeRootObject[QStringLiteral("path")] = QJsonValue(result->inputUrl());
    writeRootObject[QStringLiteral("mimetype")] = result->inputMimetype();
    writeData.setObject(writeRootObject);

    QProcess writerProcess;
    writerProcess.start(d->mainPath, QIODevice::ReadWrite);
    writerProcess.write(writeData.toJson());
    writerProcess.closeWriteChannel();
    writerProcess.waitForFinished(5000);

    output = writerProcess.readAll();

    qDebug() << "Extractor says: " << output;

    if (writerProcess.exitStatus()) {
        qDebug() << "Something went wrong while trying to extract data";
        return;
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
