//
// Copyright (C) 2018~2018 by xuzhao9 <i@xuzhao.net>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING. If not,
// see <http://www.gnu.org/licenses/>.
//
#ifndef FCITX_RIME_SCHEMA_MODEL_H
#define FCITX_RIME_SCHEMA_MODEL_H

#include <QVector>
#include <rime_api.h>

namespace fcitx_rime {
class FcitxRimeSchema {
public:
    QString path;
    QString id;
    QString name;
    int index; // index starts from 1, 0 means disabled
    bool active;
};

class SchemaModel {
public:
    void sortSchemas();
    QVector<FcitxRimeSchema> m_schemas;
};
} // namespace fcitx_rime

#endif // FCITX_RIME_SCHEMA_MODEL_H
