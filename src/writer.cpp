#include "writer.h"
#include "writer_p.h"
#include "writerplugin.h"

using namespace KFileMetaData;

Writer::Writer()
    : d(new WriterPrivate)
{
}

Writer::~Writer()
{
    delete d->m_plugin;
    delete d;
}

void Writer::write(WriteData* data)
{
    d->m_plugin->write(data);
}

QStringList Writer::mimetypes() const
{
    return d->m_plugin->writeMimetypes();
}
