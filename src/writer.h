#ifndef KFILEMETADTA_WRITER_H
#define KFILEMETADTA_WRITER_H

#include "kfilemetadata_export.h"
#include <QStringList>

namespace KFileMetaData
{

class WriteData;
class WriterCollection;
class WriterPrivate;

class KFILEMETADATA_EXPORT Writer
{
public:
    virtual ~Writer();

    void write(const WriteData& data);
    QStringList mimetypes() const;

private:
    Writer();
    Writer(const Writer&);

    void operator =(const Writer&);

    WriterPrivate *d;
    friend class WriterCollection;
};
}

#endif // KFILEMETADTA_WRITER_H
