#ifndef _KFILEMETADTA_WRITERPLUGIN_H
#define _KFILEMETADTA_WRITERPLUGIN_H

#include "kfilemetadata_export.h"
#include <QStringList>

#include "writedata.h"

namespace KFileMetaData
{

class KFILEMETADATA_EXPORT WriterPlugin : public QObject
{
    Q_OBJECT
public:
    explicit WriterPlugin(QObject* parent);
    virtual ~WriterPlugin();

    virtual QStringList writeMimetypes() const = 0;

    virtual void write(const WriteData& data) = 0;
};
}

Q_DECLARE_INTERFACE(KFileMetaData::WriterPlugin, "org.kde.kf5.kfilemetadata.WriterPlugin")
#endif // _KFILEMETADTA_WRITERPLUGIN_H
