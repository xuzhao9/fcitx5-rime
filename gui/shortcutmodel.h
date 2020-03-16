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
#ifndef FCITX_RIME_SHORTCUT_MODEL_H
#define FCITX_RIME_SHORTCUT_MODEL_H

#include "RimeConfigParser.h"
#include <QAbstractTableModel>
#include <QVector>

namespace fcitx_rime {

static constexpr int DEFAULT_PGSZ = 5;
static constexpr int COLUMN_CNT = 3;
static constexpr char RIME_ACTIONS[][30] = {
    "Page_Up",        "Page_Down",        ".next",       "full_shape",
    "simplification", "extended_charset", "ascii_punct", "toggle_menu"};
static constexpr char DISPLAY_ACTIONS[][30] = {"Page up",
                                               "Page down",
                                               "Next",
                                               "Toggle full/half shape",
                                               "Toggle traditional/simplified",
                                               "Extended charset",
                                               "Toggle ASCII punctuation",
                                               "Toggle menu"};
static constexpr int ACTIONS_NUM =
    sizeof(RIME_ACTIONS) / sizeof(RIME_ACTIONS[0]);

class ShortcutModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ShortcutModel(QObject *parent = 0);
    ~ShortcutModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    bool addRow(const struct Keybinding &&binding);
    bool removeRows(int row, int count, const QModelIndex &index) override;
    bool editRowShortcut(int row, std::string accept);
    void load(RimeConfigParser &config);
    void save();
    QList<Keybinding> m_entries;
    static QVariant keybindingConditionToString(KeybindingCondition condition);
    static KeybindingCondition keybindingConditionFromString(QVariant when);
    static QVariant keybindingActionToString(std::string action);
    static std::string stringToKeybindingAction(QVariant action);
    static KeybindingType findActionType(std::string action);

private:
    void sortShortcuts();
};
} // namespace fcitx_rime

#endif // FCITX_RIME_SHORTCUT_MODEL_H
