#include <QDebug>
#include <QCoreApplication>
#include <QPluginLoader>
#include <QDir>

#include "writer.h"
#include "writer_p.h"
#include "writerplugin.h"
#include "writercollection.h"

using namespace KFileMetaData;

class WriterCollection::Private {
public:
    QHash<QString, Writer*> m_writers;

    QList<Writer*> allWriters() const;
};

WriterCollection::WriterCollection()
    : d(new Private)
{
    QList<Writer*> all = d->allWriters();

    foreach (Writer* writer, all) {
        foreach (const QString& type, writer->mimetypes()) {
            d->m_writers.insertMulti(type, writer);
        }
    }
}

WriterCollection::~WriterCollection()
{
    qDeleteAll(d->m_writers.values().toSet());
    delete d;
}


QList<Writer*> WriterCollection::Private::allWriters() const
{
    QStringList plugins;
    QStringList pluginPaths;

    QStringList paths = QCoreApplication::libraryPaths();
    Q_FOREACH (const QString& libraryPath, paths) {
        QString path(libraryPath + QStringLiteral("/kf5/kfilemetadata"));
        QDir dir(path);

        if (!dir.exists()) {
            continue;
        }

        QStringList entryList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        Q_FOREACH (const QString& fileName, entryList) {
            // Make sure the same plugin is not loaded twice, even if it
            // installed in two different locations
            if (plugins.contains(fileName))
                continue;

            plugins << fileName;
            pluginPaths << dir.absoluteFilePath(fileName);
        }
    }
    plugins.clear();

    QList<Writer*> Writers;
    Q_FOREACH (const QString& pluginPath, pluginPaths) {
        QPluginLoader loader(pluginPath);

        if (!loader.load()) {
            qWarning() << "Could not create Writer: " << pluginPath;
            qWarning() << loader.errorString();
            continue;
        }

        QObject* obj = loader.instance();
        if (obj) {
            WriterPlugin* plugin = qobject_cast<WriterPlugin*>(obj);
            if (plugin) {
                Writer* writer = new Writer;
                writer->d->m_plugin = plugin;

                Writers << writer;
            } else {
                qDebug() << "Plugin could not be converted to an WriterPlugin";
                qDebug() << pluginPath;
            }
        }
        else {
            qDebug() << "Plugin could not creaate instance" << pluginPath;
        }
    }

    return Writers;
}

QList<Writer*> WriterCollection::fetchWriters(const QString& mimetype) const
{
    QList<Writer*> plugins = d->m_writers.values(mimetype);
    if (plugins.isEmpty()) {
        auto it = d->m_writers.constBegin();
        for (; it != d->m_writers.constEnd(); it++) {
            if (mimetype.startsWith(it.key()))
                plugins << it.value();
        }
    }

    return plugins;
}



