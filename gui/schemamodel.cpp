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
#include "SchemaModel.h"
#include <algorithm>

namespace fcitx_rime {
void SchemaModel::sortSchemas() {
    std::sort(m_schemas.begin(), m_schemas.end(),
              [](const FcitxRimeSchema &a, const FcitxRimeSchema &b) -> bool {
                  // if both inactive, sort by id
                  if (a.index == 0 && b.index == 0) {
                      return a.id < b.id;
                  }
                  if (a.index == 0) {
                      return false;
                  }
                  if (b.index == 0) {
                      return true;
                  }
                  return (a.index < b.index);
              });
}

} // namespace fcitx_rime
