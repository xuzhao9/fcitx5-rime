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

#include "ShortcutModel.h"
#include "Common.h"
#include "keynametable.h"
#include <algorithm>
#include <fcitxqtkeysequencewidget.h>

namespace fcitx_rime {

KeybindingType ShortcutModel::findActionType(std::string action) {
    if (action == ".next") {
        return KeybindingType::Select;
    }
    if (action == "ascii_mode" || action == "extended_charset" ||
        action == "simplification" || action == "full_shape" ||
        action == "ascii_punct" || action == "Toggle menu") {
        return KeybindingType::Toggle;
    }
    return KeybindingType::Send;
}

QVariant ShortcutModel::keybindingActionToString(std::string action) {
    for (int i = 0; i < ACTIONS_NUM; i++) {
        if (action == RIME_ACTIONS[i]) {
            return _(DISPLAY_ACTIONS[i]);
        }
    }
    return _(action.c_str());
}

std::string ShortcutModel::stringToKeybindingAction(QVariant action) {
    for (int i = 0; i < ACTIONS_NUM; i++) {
        if (action == _(DISPLAY_ACTIONS[i])) {
            return RIME_ACTIONS[i];
        }
    }
    return action.toString().toStdString();
}

ShortcutModel::ShortcutModel(QObject *parent) : QAbstractTableModel(parent) {}

ShortcutModel::~ShortcutModel() {}

int ShortcutModel::rowCount(const QModelIndex &parent) const {
    return m_entries.size();
}

int ShortcutModel::columnCount(const QModelIndex &parent) const {
    return COLUMN_CNT;
}

QVariant
ShortcutModel::keybindingConditionToString(KeybindingCondition condition) {
    switch (condition) {
    case KeybindingCondition::Composing:
        return _("Composing");
    case KeybindingCondition::HasMenu:
        return _("Has menu");
    case KeybindingCondition::Always:
        return _("Always");
    case KeybindingCondition::Paging:
        return _("Paging");
    }
    return "";
}

KeybindingCondition
ShortcutModel::keybindingConditionFromString(QVariant when) {
    if (when == _("Composing")) {
        return KeybindingCondition::Composing;
    }
    if (when == _("Has menu")) {
        return KeybindingCondition::HasMenu;
    }
    if (when == _("Paging")) {
        return KeybindingCondition::Paging;
    }
    if (when == _("Always")) {
        return KeybindingCondition::Always;
    }
    return KeybindingCondition::Always;
}

QVariant ShortcutModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() >= m_entries.size() || index.column() >= COLUMN_CNT) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return keybindingActionToString(m_entries[index.row()].action);
        case 1:
            return keybindingConditionToString(m_entries[index.row()].when);
        case 2:
            return QString::fromStdString(m_entries[index.row()].accept);
        }
    }
    return QVariant();
}

QVariant ShortcutModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
    if (orientation == Qt::Vertical) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    switch (role) {
    case Qt::DisplayRole:
        switch (section) {
        case 0:
            return _("Function");
        case 1:
            return _("Input Mode");
        case 2:
            return _("Key");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

void ShortcutModel::load(RimeConfigParser &config) {
    beginResetModel();
    m_entries.clear();
    int page_size = DEFAULT_PGSZ;
    bool suc = config.readInteger("menu/page_size", &page_size);
    if (!suc) {
        page_size = DEFAULT_PGSZ;
    }
    // Add toggle_menu shortcuts
    auto toggleKeys = config.toggleKeys();
    for (auto &toggleKey : toggleKeys) {
        struct Keybinding toggle_binding;
        toggle_binding.when = KeybindingCondition::Always;
        toggle_binding.accept = toggleKey;
        toggle_binding.type = KeybindingType::Toggle;
        toggle_binding.action = "Toggle menu";
        m_entries.push_back(std::move(toggle_binding));
    }

    // Add number of candidates
    struct Keybinding num_candids;
    num_candids.when = KeybindingCondition::Always;
    num_candids.accept = std::to_string(page_size);
    num_candids.type = KeybindingType::Send;
    num_candids.action = "Number of candidates";
    m_entries.push_back(num_candids);

    // Add other keybindings
    auto bindings = config.keybindings();
    for (const auto &binding : bindings) {
        if (binding.accept.empty()) {
            continue;
        }
        m_entries.push_back(std::move(binding));
    }

    endResetModel();
}

void ShortcutModel::save() {}

void ShortcutModel::sortShortcuts() {
    std::sort(
        m_entries.begin(), m_entries.end(),
        [](const struct Keybinding &a, const struct Keybinding &b) -> bool {
            if (a.when == b.when) {
                return a.action < b.action;
            } else {
                return a.when > b.when;
            }
        });
}

bool ShortcutModel::addRow(const struct Keybinding &&entry) {
    for (int i = 0; i < m_entries.size(); i++) {
        if (entry.accept == m_entries[i].accept) {
            return false; // Key conflict
        }
    }
    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size() + 1);
    m_entries.push_back(entry);
    endInsertRows();
    return true;
}

bool ShortcutModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row < 0 || row >= m_entries.size()) {
        return false;
    }
    if (row + count - 1 >= m_entries.size()) {
        return false;
    }
    beginRemoveRows(parent, row, (row + count - 1));
    for (int i = 0; i < count; i++) {
        m_entries.removeAt(row + i);
    }
    endRemoveRows();
    return true;
}

bool ShortcutModel::editRowShortcut(int row, std::string accept) {
    if (row < 0 || row >= m_entries.size()) {
        return false;
    }
    for (int i = 0; i < m_entries.size(); i++) {
        if (i == row) {
            continue;
        }
        if (accept == m_entries[i].accept) {
            return false; // Key conflict
        }
    }
    m_entries[row].accept = accept;
    return true;
}

} // namespace fcitx_rime
