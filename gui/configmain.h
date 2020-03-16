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
#ifndef FCITX_RIME_CONFIGMAIN_H
#define FCITX_RIME_CONFIGMAIN_H

#include <fcitxqtconfiguiwidget.h>
#include <fcitxqtkeysequencewidget.h>

#include "configparser.h"
#include "schemamodel.h"
#include "shortcutmodel.h"
#include "ui_configmain.h"

namespace fcitx_rime {

class ConfigMain : public FcitxQtConfigUIWidget, private Ui::MainUI {
    Q_OBJECT
public:
    explicit ConfigMain(QWidget *parent = 0);
    QString title() override;
    ~ConfigMain();
    void load() override;
    void save() override;
    bool asyncSave() override { return true; }

    QString addon() override;
    QString icon() override;
public slots:
    void stateChanged();
    void addIM();
    void removeIM();
    void moveUpIM();
    void moveDownIM();
    void addShortcut();
    void removeShortcut();
    void editShortcut();
    void availIMSelectionChanged();
    void activeIMSelectionChanged();

private:
    void yamlToModel();
    void uiToModel();
    void modelToUi();
    void modelToYaml();
    void getAvailableSchemas();
    void updateIMList();
    void focusSelectedIM(const QString im_name);

    RimeConfigParser config;
    SchemaModel *m_schemamodel;
    ShortcutModel *m_shortcutmodel;
};
} // namespace fcitx_rime

#endif // FCITX_RIME_CONFIGMAIN_H