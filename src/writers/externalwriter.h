#ifndef EXTERNALWRITER_H
#define EXTERNALWRITER_H

#include "writerplugin.h"
#include <QMultiHash>

namespace KFileMetaData
{

class ExternalWriter : public WriterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kf5.kfilemetadata.WriterPlugin")
    Q_INTERFACES(KFileMetaData::WriterPlugin)

public:
    ExternalWriter(QObject* parent = 0);

    QStringList writeMimetypes() const Q_DECL_OVERRIDE;
    void write(const WriteData& data) Q_DECL_OVERRIDE;

private:
    static QStringList findWriters();

    QMultiHash<QString, QString> m_writers;
    QTextCodec *m_textCodec;
};

}

#endif // EXTERNALWRITER_H
