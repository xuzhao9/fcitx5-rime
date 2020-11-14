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
#include "KeySeq.h"
#include "Common.h"
#include "keynametable.h"
#include <fcitxqtkeysequencewidget.h>

namespace fcitx_rime {

KeySeq::KeySeq() {}

// convert keyseq to state and sym
KeySeq::KeySeq(const char *keyseq) {
    KeyStates states;
    const char *p = keyseq;
    const char *lastModifier = keyseq;
    const char *found = nullptr;
// use macro to check modifiers
#define _CHECK_MODIFIER(NAME, VALUE)                                           \
    if ((found = strstr(p, NAME))) {                                           \
        states |= fcitx::KeyState::VALUE;                                      \
        if (found + strlen(NAME) > lastModifier) {                             \
            lastModifier = found + strlen(NAME);                               \
        }                                                                      \
    }
    _CHECK_MODIFIER("CTRL_", Ctrl)
    _CHECK_MODIFIER("Control+", Ctrl)
    _CHECK_MODIFIER("ALT_", Alt)
    _CHECK_MODIFIER("Alt+", Alt)
    _CHECK_MODIFIER("SHIFT_", Shift)
    _CHECK_MODIFIER("Shift+", Shift)
    _CHECK_MODIFIER("SUPER_", Super)
    _CHECK_MODIFIER("Super+", Super)

#undef _CHECK_MODIFIER
    sym_ = keySymFromString(lastModifier);
    states_ = states;
}

KeySym KeySeq::keySymFromString(const char *keyString) {
    auto value = std::lower_bound(
        keyValueByNameOffset,
        keyValueByNameOffset + FCITX_RIME_ARRAY_SIZE(keyValueByNameOffset),
        keyString, [](const uint32_t &idx, const std::string &str) {
            return keyNameList[&idx - keyValueByNameOffset] < str;
        });
    if (value != keyValueByNameOffset +
                     FCITX_RIME_ARRAY_SIZE(keyValueByNameOffset) &&
        strcmp(keyString, keyNameList[value - keyValueByNameOffset]) == 0) {
        return static_cast<KeySym>(*value);
    }

    return FcitxKey_None;
}

std::string KeySeq::keySymToString(KeySym sym) const {
    const KeyNameOffsetByValue *result = std::lower_bound(
        keyNameOffsetByValue,
        keyNameOffsetByValue + FCITX_RIME_ARRAY_SIZE(keyNameOffsetByValue), sym,
        [](const KeyNameOffsetByValue &item, KeySym key) {
            return item.sym < key;
        });
    if (result != keyNameOffsetByValue +
                      FCITX_RIME_ARRAY_SIZE(keyNameOffsetByValue) &&
        result->sym == sym) {
        return keyNameList[result->offset];
    }
    return std::string();
}

// convert QKeySequence to state and sym
KeySeq::KeySeq(const QKeySequence qkey) {
    int sym = 0;
    uint states = 0;
    int qkeycode = static_cast<int>(qkey[0]);
    FcitxQtKeySequenceWidget::keyQtToFcitx(
        qkeycode, FcitxQtModifierSide::MS_Unknown, sym, states);
    sym_ = static_cast<FcitxKeySym>(sym);
    states_ = static_cast<fcitx::KeyState>(states);
}

// convert to Rime X11 style string
std::string KeySeq::toString() const {
    auto sym = sym_;
    if (sym == FcitxKey_None) {
        return std::string();
    }
    if (sym == FcitxKey_ISO_Left_Tab) {
        sym = FcitxKey_Tab;
    }

    auto key = keySymToString(sym);

    if (key.empty()) {
        return std::string();
    }

    std::string str;

#define _APPEND_MODIFIER_STRING(STR, VALUE)                                    \
    if (states_ & fcitx::KeyState::VALUE) {                                    \
        str += STR;                                                            \
    }

    _APPEND_MODIFIER_STRING("Control+", Ctrl)
    _APPEND_MODIFIER_STRING("Alt+", Alt)
    _APPEND_MODIFIER_STRING("Shift+", Shift)
    _APPEND_MODIFIER_STRING("Super+", Super)

#undef _APPEND_MODIFIER_STRING
    str += key;
    return str;
}

QKeySequence KeySeq::toQKeySequence() {
    return QKeySequence(FcitxQtKeySequenceWidget::keyFcitxToQt(sym_, states_));
}

} // namespace fcitx_rime
