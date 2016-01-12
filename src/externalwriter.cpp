#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>

#include "externalwriter.h"
#include "properties.h"
#include "propertyinfo.h"

using namespace KFileMetaData;

class ExternalWriter::Private {
public:
    QString path;
    QStringList writeMimetypes;
    QString language;
};


ExternalWriter::ExternalWriter(QObject* parent)
    : WriterPlugin(parent)
{
}

ExternalWriter::ExternalWriter(const QString& pluginPath)
    : WriterPlugin(new QObject())
{
    d->path = pluginPath;

    QDir pluginDir(pluginPath);
    QStringList pluginDirContents = pluginDir.entryList();

    if (!pluginDirContents.contains(QStringLiteral("manifest.json"))) {
        qDebug() << "Path does not seem to contain a valid plugin";
    }
}

QStringList ExternalWriter::writeMimetypes() const
{
    return d->writeMimetypes;
}

void ExternalWriter::write(const WriteData& data)
{
    QJsonDocument writeData;
    QJsonObject rootObject = writeData.object();
    QJsonObject propertiesObject;

    QMap<Property::Property, QVariant> properties = data.getAllProperties();

    Q_FOREACH(Property::Property property, properties.keys()) {
        PropertyInfo propertyInfo(property);
        propertiesObject[propertyInfo.name()] = QJsonValue::fromVariant(properties[property]);
    }

    rootObject[QStringLiteral("path")] = QJsonValue(data.inputUrl());
    rootObject[QStringLiteral("mimetype")] = data.inputMimetype();
    rootObject[QStringLiteral("properties")] = propertiesObject;
    QJsonValue(writeData).toString();

    QStringList writeArgs({ QString(writeData.toJson(QJsonDocument::Compact)) });
    QProcess writerProcess;
    writerProcess.start(d->path, writeArgs, QIODevice::ReadWrite);
    writerProcess.waitForFinished(5000);

    if (writerProcess.exitStatus()) {
        qDebug() << "Something went wrong while executing " + d->path;
        return;
    }

}
