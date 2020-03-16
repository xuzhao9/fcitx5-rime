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
#ifndef FCITX_RIME_KEY_SEQ_H
#define FCITX_RIME_KEY_SEQ_H

#include "keysym.h"
#include "keysymgen.h"
#include <QFlags>
#include <QKeySequence>

namespace fcitx_rime {
typedef QFlags<fcitx::KeyState> KeyStates;
typedef FcitxKeySym KeySym;
class KeySeq {
public:
    KeyStates states_;
    KeySym sym_;
    KeySeq();
    KeySeq(const char *keyseq);
    KeySeq(const std::string &keyseq) : KeySeq(keyseq.data()) {}
    KeySeq(const QKeySequence qkey);
    std::string toString() const;
    std::string keySymToString(KeySym sym) const;
    KeySym keySymFromString(const char *keyString);
    QKeySequence toQKeySequence();
};
} // namespace fcitx_rime
#endif // FCITX_RIME_KEY_SEQ_H
