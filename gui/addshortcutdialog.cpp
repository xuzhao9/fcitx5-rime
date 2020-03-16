//
// Copyright (C) 2020 by xz <i@xuzhao.net>
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

#include "addshortcutdialog.h"
#include "common.h"
#include "keyseq.h"
#include "shortcutmodel.h"

#include <iostream>

namespace fcitx_rime {

AddShortcutDialog::AddShortcutDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    for (int i = 0; i < ACTIONS_NUM; i++) {
        commandBox->addItem(_(DISPLAY_ACTIONS[i]));
    }
    inputModeBox->addItem(ShortcutModel::keybindingConditionToString(
                              KeybindingCondition::Composing)
                              .toString());
    inputModeBox->addItem(
        ShortcutModel::keybindingConditionToString(KeybindingCondition::HasMenu)
            .toString());
    inputModeBox->addItem(
        ShortcutModel::keybindingConditionToString(KeybindingCondition::Always)
            .toString());
    inputModeBox->addItem(
        ShortcutModel::keybindingConditionToString(KeybindingCondition::Paging)
            .toString());
}

AddShortcutDialog::AddShortcutDialog(const Keybinding &binding, QWidget *parent)
    : QDialog(parent) {
    setupUi(this);
    commandBox->addItem(
        ShortcutModel::keybindingActionToString(binding.action).toString());
    inputModeBox->addItem(
        ShortcutModel::keybindingConditionToString(binding.when).toString());
    if (binding.action == "Number of candidates") {
        keyLabel->setVisible(false);
        keyButton->setVisible(false);
        candidLabel->setVisible(true);
        candidVal->setVisible(true);
        candidVal->setValue(std::stoi(binding.accept));
    } else {
        keyButton->setKeySequence(KeySeq(binding.accept).toQKeySequence());
    }
}

AddShortcutDialog::~AddShortcutDialog() {}

std::string AddShortcutDialog::getShortcut() const {
    if (candidVal->value()) {
        return std::to_string(candidVal->value());
    } else {
        KeySeq seq = KeySeq(keyButton->keySequence());
        return seq.toString();
    }
}

Keybinding AddShortcutDialog::getKeybinding() const {
    struct Keybinding binding;
    binding.when = ShortcutModel::keybindingConditionFromString(
        inputModeBox->itemData(inputModeBox->currentIndex()));
    binding.accept = getShortcut();
    binding.action = ShortcutModel::stringToKeybindingAction(
        commandBox->itemText(commandBox->currentIndex()));
    binding.type = ShortcutModel::findActionType(binding.action);
    return binding;
}
} // namespace fcitx_rime
