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

#ifndef FCITX_RIME_ADD_SHORTCUT_DIALOG_H
#define FCITX_RIME_ADD_SHORTCUT_DIALOG_H_

#include "configparser.h"
#include "ui_addshortcutdialog.h"

#include <QDialog>

namespace fcitx_rime {
class AddShortcutDialog : public QDialog, private Ui::AddShortcutDialog {
    Q_OBJECT
public:
    explicit AddShortcutDialog(QWidget *parent = 0);
    explicit AddShortcutDialog(const Keybinding &binding, QWidget *parent = 0);
    ~AddShortcutDialog();
    std::string getShortcut() const;
    Keybinding getKeybinding() const;
};
} // namespace fcitx_rime

#endif // FCITX_RIME_ADD_SHORTCUT_DIALOG_H
