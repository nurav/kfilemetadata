/*
 * This file is part of the KFileMetaData project
 * Copyright (C) 2016  Varun Joshi <varunj.1011@gmail.com>
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

#include "externalwriter.h"
#include "properties.h"
#include "propertyinfo.h"

using namespace KFileMetaData;

class ExternalWriter::Private {
public:
    QString path;
    QStringList writeMimetypes;
    QString mainPath;
};


ExternalWriter::ExternalWriter(QObject* parent)
    : WriterPlugin(parent)
{
}

ExternalWriter::ExternalWriter(const QString& pluginPath)
    : WriterPlugin(new QObject()),
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

QStringList ExternalWriter::writeMimetypes() const
{
    return d->writeMimetypes;
}

void ExternalWriter::write(const WriteData& data)
{
    QJsonDocument writeData;
    QJsonObject rootObject;
    QJsonObject propertiesObject;

    QMap<Property::Property, QVariant> properties = data.getAllProperties();

    Q_FOREACH(Property::Property property, properties.keys()) {
        PropertyInfo propertyInfo(property);
        propertiesObject[propertyInfo.name()] = QJsonValue::fromVariant(properties[property]);
    }

    rootObject[QStringLiteral("path")] = QJsonValue(data.inputUrl());
    rootObject[QStringLiteral("mimetype")] = data.inputMimetype();
    rootObject[QStringLiteral("properties")] = propertiesObject;
    writeData.setObject(rootObject);

    QProcess writerProcess;
    writerProcess.start(d->mainPath, QIODevice::ReadWrite);
    writerProcess.write(writeData.toJson());
    writerProcess.closeWriteChannel();
    writerProcess.waitForFinished(5000);

    qDebug() << "Writer says: " + writerProcess.readAll();

    if (writerProcess.exitStatus()) {
        qDebug() << "Something went wrong while trying to write data";
        return;
    }

}
