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
    QString language;
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
    d->language = rootObject[QStringLiteral("language")].toString();
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

    QStringList writeArgs({ QString::fromUtf8(writeData.toJson()) });
    QProcess writerProcess;

    QString execCommand;

    if (d->language == QStringLiteral("python")) {
        execCommand = QStringLiteral("python ");
    }
    execCommand += d->mainPath;
    writerProcess.start(d->mainPath, writeArgs, QIODevice::ReadOnly);
    writerProcess.waitForFinished(5000);

    qDebug() << "Writer says: " + writerProcess.readAll();

    if (writerProcess.exitStatus()) {
        qDebug() << "Something went wrong while trying to write data";
        return;
    }

}
