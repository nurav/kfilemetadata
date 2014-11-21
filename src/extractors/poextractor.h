/*
    Gettext translation file analyzer

    Copyright (C) 2007 Montel Laurent <montel@kde.org>
    Copyright (C) 2009 Jos van den Oever <jos@vandenoever.info>
    Copyright (C) 2014 Nick Shaforostoff <shaforostoff@gmail.com>

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


#ifndef POEXTRACTOR_H
#define POEXTRACTOR_H

#include "extractorplugin.h"

namespace KFileMetaData
{

class POExtractor : public ExtractorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kf5.kfilemetadata.ExtractorPlugin")
    Q_INTERFACES(KFileMetaData::ExtractorPlugin)

public:
    POExtractor(QObject* parent = 0);

    virtual QStringList mimetypes() const;
    virtual void extract(ExtractionResult* result);

private:
    void endMessage();
    void handleComment(const char* data, uint32_t length);
    void handleLine(const char* data, uint32_t length);


    enum PoState {COMMENT, MSGCTXT, MSGID, MSGID_PLURAL, MSGSTR, MSGSTR_PLURAL,
        WHITESPACE, ERROR};
    PoState state;
    int messages;
    int untranslated;
    int fuzzy;
    bool isFuzzy=false, isTranslated=false;
};

}

#endif // PLAINTEXTEXTRACTOR_H
