/*
 * TagLibExtractor tests.
 *
 * Copyright (C) 2015  Juan Palacios <jpalaciosdev@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

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

    virtual void write(WriteData* data) = 0;
};
}

Q_DECLARE_INTERFACE(KFileMetaData::WriterPlugin, "org.kde.kf5.kfilemetadata.WriterPlugin")
#endif // _KFILEMETADTA_WRITERPLUGIN_H
