/*
    A conduit between KFileMetaData's native plugin system and plugins written
    in other languages, installed as binaries on the user's system

    Copyright (C) 2015  Boudhayan Gupta <bgupta@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef EXTERNALEXTRACTOR_H
#define EXTERNALEXTRACTOR_H

#include "extractorplugin.h"
#include <QMultiHash>

namespace KFileMetaData
{

class ExternalExtractor : public ExtractorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kf5.kfilemetadata.ExtractorPlugin")
    Q_INTERFACES(KFileMetaData::ExtractorPlugin)

public:
    ExternalExtractor(QObject* parent = 0);

    QStringList mimetypes() const Q_DECL_OVERRIDE;
    void extract(ExtractionResult* result) Q_DECL_OVERRIDE;

private:
    static QStringList findExtractors();

    QMultiHash<QString, QString> m_extractors;
    QTextCodec *m_textCodec;
};

}

#endif // EXTERNALEXTRACTOR_H
