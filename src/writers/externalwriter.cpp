#include "externalwriter.h"
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

QStringList ExternalWriter::findWriters()
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

ExternalWriter::ExternalWriter(QObject *parent) :
    WriterPlugin(parent),
    m_writers(QMultiHash<QString, QString>()),
    m_textCodec(QTextCodec::codecForLocale())
{
    // we try to find all kfilemetadata_extractor_* binaries in PATH, and run them
    // with the --mimetypes flag to see what mimetypes they support

    Q_FOREACH(auto extractor, findWriters()) {
        QProcess process;
        process.start(extractor, QStringList({ QStringLiteral("--mimetypes") }), QIODevice::ReadOnly);
        process.waitForFinished();

        while (!process.atEnd()) {
            QByteArray data = process.readLine();
            QString mimeType = m_textCodec->toUnicode(data);
            mimeType = mimeType.trimmed();
            m_writers.insert(mimeType, extractor);
        }
    }
}

QStringList ExternalWriter::writeMimetypes() const
{
    return m_writers.keys();
}

void ExternalWriter::write(const WriteData& data)
{
//    QString extractor = m_writers.value(result->inputMimetype());
//    QStringList filename({ data.inputUrl() });
//    QByteArray output;

//    // we start by running the extractor on the file and capturing all the
//    // output

//    {
//        QProcess extractorProcess;
//        extractorProcess.start(extractor, filename, QIODevice::ReadOnly);
//        extractorProcess.waitForFinished();

//        if (extractorProcess.exitStatus()) {
//            return;
//        }
//        output = extractorProcess.readAll();
//    }

//    // now we read in the output (which is a standard json format) into the
//    // ExtractionResult

//    QJsonDocument extractorData = QJsonDocument::fromJson(output);
//    if (!extractorData.isObject()) {
//        return;
//    }
//    QJsonObject rootObject = extractorData.object();

//    Q_FOREACH(auto key, rootObject.keys()) {
//        if (key == QStringLiteral("typeInfo")) {
//            TypeInfo info = TypeInfo::fromName(rootObject.value(key).toString());
//            result->addType(info.type());
//            continue;
//        }

//        PropertyInfo info = PropertyInfo::fromName(key);
//        if (info.name() != key) {
//            continue;
//        }
//        result->add(info.property(), rootObject.value(key).toVariant());
//    }
}
