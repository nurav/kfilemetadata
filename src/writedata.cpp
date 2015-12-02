#include <QMimeDatabase>

#include "writedata.h"

using namespace KFileMetaData;

class WriteData::Private {
public:
    QString url;
    QString mimetype;
};

WriteData::WriteData(const QString& url, const QString& mimetype)
    : d(new Private)
{
    d->url = url;
    d->mimetype = mimetype;
    if (mimetype.isEmpty()) {
        d->mimetype = QMimeDatabase().mimeTypeForFile(url).name();
    }
}

WriteData::WriteData(const WriteData& rhs)
    : d(new Private(*rhs.d))
{
}

WriteData::~WriteData()
{
    delete d;
}

QString WriteData::inputUrl() const
{
    return d->url;
}

QString WriteData::inputMimetype() const
{
    return d->mimetype;
}

