#ifndef _KFILEMETADATA_WRITEDATA_H
#define _KFILEMETADATA_WRITEDATA_H

#include "kfilemetadata_export.h"

#include <QString>
#include <QMap>
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
    WriteData& operator=(const WriteData& rhs);
    bool operator==(const WriteData& rhs) const;
    
    QString inputUrl() const;

    QString inputMimetype() const;

    void add(Property::Property property, const QVariant& value);

    QMap<Property::Property, QVariant> getAllProperties() const;

private:
    class Private;
    Private* d;
};
}

#endif // _KFILEMETADATA_WRITEDATA_H
