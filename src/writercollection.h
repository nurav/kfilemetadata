#ifndef _KFILEMETADTA_WRITERCOLLECTION_H
#define _KFILEMETADTA_WRITERCOLLECTION_H

#include <QList>

#include "kfilemetadata_export.h"
#include "writer.h"

namespace KFileMetaData
{

class KFILEMETADATA_EXPORT WriterCollection
{
public:
    explicit WriterCollection();
    virtual ~WriterCollection();

    QList<Writer*> fetchWriters(const QString& mimetype) const;

private:
    class Private;
    Private* d;
};
}

#endif // _KFILEMETADTA_WRITERCOLLECTION_H
