#ifndef _KFILEMETADATA_WRITEDATA_H
#define _KFILEMETADATA_WRITEDATA_H

#include "kfilemetadata_export.h"

#include <QString>
#include <QVariant>

#include "properties.h"
#include "types.h"

namespace KFileMetaData {

class KFILEMETADATA_EXPORT WriteData
{
public:
    WriteData(const QString& url, const QString& mimetype);
    WriteData(const WriteData& rhs);
    virtual ~WriteData();
    
    QString inputUrl() const;

    QString inputMimetype() const;

    virtual void add(Property::Property property, const QVariant& value) = 0;

    virtual void addType(Type::Type type) = 0;

    virtual PropertyMap getAllProperties() = 0;

private:
    class Private;
    Private* d;
};
}

#endif // _KFILEMETADATA_WRITEDATA_H
