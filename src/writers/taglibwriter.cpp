#include "taglibwriter.h"

#include <taglib.h>
#include <tstring.h>
#include <id3v2tag.h>
#include <fileref.h>

using namespace KFileMetaData;

TagLibWriter::TagLibWriter(QObject* parent)
    : WriterPlugin(parent)
{
}

TagLib::String q2t(const QString& q)
{
    return TagLib::String(q.toStdWString());
}

QStringList TagLibWriter::writeMimetypes() const
{
    QStringList types;

    types << QStringLiteral("audio/mpeg");
    types << QStringLiteral("audio/mpeg3"); types << QStringLiteral("audio/x-mpeg");

    types << QStringLiteral("audio/opus"); types << QStringLiteral("audio/x-opus+ogg");

    return types;
}

void TagLibWriter::write(WriteData *data)
{
    const QString fileUrl = data->inputUrl();
    const QString mimeType = data->inputMimetype();
    const PropertyMap properties = data->getAllProperties();

    TagLib::FileRef file(fileUrl.toUtf8().constData(), true);
    if (file.isNull()) {
        return;
    }

    TagLib::Tag* tags = file.tag();

    TagLib::String title;
    TagLib::String artists;

    if (properties.contains(Property::Title)) {
        title = q2t(properties[Property::Title].toString());
        tags->setTitle(title);
    }

    if (properties.contains(Property::Artist)) {
        artists = q2t(properties[Property::Artist].toString());
        tags->setArtist(artists);
    }

    file.save();
}
