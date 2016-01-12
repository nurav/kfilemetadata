#ifndef EXTERNALWRITER_H
#define EXTERNALWRITER_H

#include "writerplugin.h"

namespace KFileMetaData {

class ExternalWriter : public WriterPlugin
{
public:
    ExternalWriter(QObject* parent = 0);
    ExternalWriter(const QString& pluginPath);

    void write(const WriteData& data) Q_DECL_OVERRIDE;
    QStringList writeMimetypes() const Q_DECL_OVERRIDE;

private:
    bool runtimeInstalled() const;
    bool dependenciesSatisfied() const;

    class Private;
    Private *d;
};
}

#endif // EXTERNALWRITER_H
