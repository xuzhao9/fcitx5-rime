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
#include <fcitx-config/xdg.h>

#include "addshortcutdialog.h"
#include "common.h"
#include "configmain.h"
#include <QDialogButtonBox>
#include <QDir>
#include <QFutureWatcher>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QtConcurrentRun>
#include <QtGlobal>

// TODO: when failed-read happens, disable ui
// TODO: when failed-save happens, disable ui and show reason

namespace fcitx_rime {
ConfigMain::ConfigMain(QWidget *parent)
    : FcitxQtConfigUIWidget(parent), m_schemamodel(new SchemaModel()),
      m_shortcutmodel(new ShortcutModel(this)) {
    // Setup UI
    setMinimumSize(680, 500);
    setupUi(this);

    // Setup schema tab
    addIMButton->setIcon(QIcon::fromTheme("go-next"));
    removeIMButton->setIcon(QIcon::fromTheme("go-previous"));
    moveUpButton->setIcon(QIcon::fromTheme("go-up"));
    moveDownButton->setIcon(QIcon::fromTheme("go-down"));
    QStandardItemModel *listModel = new QStandardItemModel(this);
    currentIMView->setModel(listModel);
    QStandardItemModel *availIMModel = new QStandardItemModel(this);
    availIMView->setModel(availIMModel);
    connect(removeIMButton, &QPushButton::clicked, this, &ConfigMain::removeIM);
    connect(addIMButton, &QPushButton::clicked, this, &ConfigMain::addIM);
    connect(moveUpButton, &QPushButton::clicked, this, &ConfigMain::moveUpIM);
    connect(moveDownButton, &QPushButton::clicked, this,
            &ConfigMain::moveDownIM);
    connect(availIMView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ConfigMain::availIMSelectionChanged);
    connect(currentIMView->selectionModel(),
            &QItemSelectionModel::currentChanged, this,
            &ConfigMain::activeIMSelectionChanged);

    // Setup shortcut tab
    shortcutView->setModel(m_shortcutmodel);
    shortcutView->setColumnWidth(0, 180);
    shortcutView->sortByColumn(3);
    connect(addShortcutButton, &QPushButton::clicked, this,
            &ConfigMain::addShortcut);
    connect(removeShortcutButton, &QPushButton::clicked, this,
            &ConfigMain::removeShortcut);
    connect(editShortcutButton, &QPushButton::clicked, this,
            &ConfigMain::editShortcut);

    // Load user config
    yamlToModel();
    modelToUi();
}

ConfigMain::~ConfigMain() {
    delete m_schemamodel;
    delete m_shortcutmodel;
}

// SLOTs
void ConfigMain::stateChanged() { emit changed(true); }

void ConfigMain::focusSelectedIM(const QString im_name) {
    // search enabled IM first
    int sz = currentIMView->model()->rowCount();
    for (int i = 0; i < sz; i++) {
        QModelIndex ind = currentIMView->model()->index(i, 0);
        const QString name =
            currentIMView->model()->data(ind, Qt::DisplayRole).toString();
        if (name == im_name) {
            currentIMView->setCurrentIndex(ind);
            currentIMView->setFocus();
            return;
        }
    }
    // if not found, search avali IM list
    sz = availIMView->model()->rowCount();
    for (int i = 0; i < sz; i++) {
        QModelIndex ind = availIMView->model()->index(i, 0);
        const QString name =
            availIMView->model()->data(ind, Qt::DisplayRole).toString();
        if (name == im_name) {
            availIMView->setCurrentIndex(ind);
            availIMView->setFocus();
            return;
        }
    }
}

void ConfigMain::addIM() {
    if (availIMView->currentIndex().isValid()) {
        const QString uniqueName =
            availIMView->currentIndex().data(Qt::DisplayRole).toString();
        int largest = 0;
        int find = -1;
        for (size_t i = 0; i < m_schemamodel->m_schemas.size(); i++) {
            if (m_schemamodel->m_schemas[i].name == uniqueName) {
                find = i;
            }
            if (m_schemamodel->m_schemas[i].index > largest) {
                largest = m_schemamodel->m_schemas[i].index;
            }
        }
        if (find != -1) {
            m_schemamodel->m_schemas[find].active = true;
            m_schemamodel->m_schemas[find].index = largest + 1;
        }

        m_schemamodel->sortSchemas();
        updateIMList();
        focusSelectedIM(uniqueName);
        stateChanged();
    }
}

void ConfigMain::removeIM() {
    if (currentIMView->currentIndex().isValid()) {
        const QString uniqueName =
            currentIMView->currentIndex().data(Qt::DisplayRole).toString();
        for (size_t i = 0; i < m_schemamodel->m_schemas.size(); i++) {
            if (m_schemamodel->m_schemas[i].name == uniqueName) {
                m_schemamodel->m_schemas[i].active = false;
                m_schemamodel->m_schemas[i].index = 0;
            }
        }
        m_schemamodel->sortSchemas();
        updateIMList();
        focusSelectedIM(uniqueName);
        stateChanged();
    }
}

void ConfigMain::moveUpIM() {
    if (currentIMView->currentIndex().isValid()) {
        const QString uniqueName =
            currentIMView->currentIndex().data(Qt::DisplayRole).toString();
        int cur_index = -1;
        for (size_t i = 0; i < m_schemamodel->m_schemas.size(); i++) {
            if (m_schemamodel->m_schemas[i].name == uniqueName) {
                cur_index = m_schemamodel->m_schemas[i].index;
                Q_ASSERT(cur_index ==
                         (i + 1)); // make sure the schema is sorted
            }
        }
        // can't move up the top schema because the button should be grey
        if (cur_index == -1 || cur_index == 0) {
            return;
        }

        int temp;
        temp = m_schemamodel->m_schemas[cur_index - 1].index;
        m_schemamodel->m_schemas[cur_index - 1].index =
            m_schemamodel->m_schemas[cur_index - 2].index;
        m_schemamodel->m_schemas[cur_index - 2].index = temp;
        m_schemamodel->sortSchemas();
        updateIMList();
        focusSelectedIM(uniqueName);
        stateChanged();
    }
}

void ConfigMain::moveDownIM() {
    if (currentIMView->currentIndex().isValid()) {
        const QString uniqueName =
            currentIMView->currentIndex().data(Qt::DisplayRole).toString();
        int cur_index = -1;
        for (size_t i = 0; i < m_schemamodel->m_schemas.size(); i++) {
            if (m_schemamodel->m_schemas[i].name == uniqueName) {
                cur_index = m_schemamodel->m_schemas[i].index;
                Q_ASSERT(cur_index ==
                         (i + 1)); // make sure the schema is sorted
            }
        }
        // can't move down the bottom schema because the button should be grey
        if (cur_index == -1 || cur_index == 0) {
            return;
        }
        int temp;
        temp = m_schemamodel->m_schemas[cur_index - 1].index;
        m_schemamodel->m_schemas[cur_index - 1].index =
            m_schemamodel->m_schemas[cur_index].index;
        m_schemamodel->m_schemas[cur_index].index = temp;
        m_schemamodel->sortSchemas();
        updateIMList();
        focusSelectedIM(uniqueName);
        stateChanged();
    }
}

void ConfigMain::addShortcut() {
    AddShortcutDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        struct Keybinding binding = dialog.getKeybinding();
        if (m_shortcutmodel->addRow(std::move(binding))) {
            shortcutView->setFocus();
            stateChanged();
        } else {
            QMessageBox::critical(
                this, _("Key Conflict"),
                _("Key to add is conflict with existing shortcut."));
        }
    }
}

void ConfigMain::removeShortcut() {
    QModelIndex select = shortcutView->currentIndex();
    if (select.isValid()) {
        shortcutView->model()->removeRows(select.row(), 1, select);
        shortcutView->setFocus();
        stateChanged();
    }
}

void ConfigMain::editShortcut() {
    QModelIndex select = shortcutView->currentIndex();
    if (select.isValid()) {
        int row = select.row();
        const Keybinding &binding = m_shortcutmodel->m_entries[row];
        AddShortcutDialog dialog(binding);
        if (dialog.exec() == QDialog::Accepted) {
            std::string accept = dialog.getShortcut();
            if (m_shortcutmodel->editRowShortcut(row, accept)) {
                shortcutView->setFocus();
                stateChanged();
            } else {
                QMessageBox::critical(
                    this, _("Key Conflict"),
                    _("Key to add is conflict with existing shortcut."));
            }
        }
    }
}

void ConfigMain::availIMSelectionChanged() {
    if (!availIMView->currentIndex().isValid()) {
        addIMButton->setEnabled(false);
    } else {
        addIMButton->setEnabled(true);
    }
}

void ConfigMain::activeIMSelectionChanged() {
    if (!currentIMView->currentIndex().isValid()) {
        removeIMButton->setEnabled(false);
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
        // configureButton->setEnabled(false);
    } else {
        removeIMButton->setEnabled(true);
        // configureButton->setEnabled(true);
        if (currentIMView->currentIndex().row() == 0) {
            moveUpButton->setEnabled(false);
        } else {
            moveUpButton->setEnabled(true);
        }
        if (currentIMView->currentIndex().row() ==
            currentIMView->model()->rowCount() - 1) {
            moveDownButton->setEnabled(false);
        } else {
            moveDownButton->setEnabled(true);
        }
    }
}
// end of SLOTs

QString ConfigMain::icon() { return "fcitx-rime"; }

QString ConfigMain::addon() { return "fcitx-rime"; }

QString ConfigMain::title() { return _("Fcitx Rime Config GUI Tool"); }

void ConfigMain::load() {}

void ConfigMain::uiToModel() {
    // Reinitialize curent model and save from the ui
    for (int i = 0; i < m_schemamodel->m_schemas.size(); i++) {
        m_schemamodel->m_schemas[i].index = 0;
        m_schemamodel->m_schemas[i].active = false;
    }
    QStandardItemModel *qmodel =
        static_cast<QStandardItemModel *>(currentIMView->model());
    QModelIndex parent;
    int seqno = 1;
    for (int r = 0; r < qmodel->rowCount(parent); ++r) {
        QModelIndex index = qmodel->index(r, 0, parent);
        QVariant name = qmodel->data(index);
        for (int i = 0; i < m_schemamodel->m_schemas.size(); i++) {
            if (m_schemamodel->m_schemas[i].name == name) {
                m_schemamodel->m_schemas[i].index = seqno++;
                m_schemamodel->m_schemas[i].active = true;
            }
        }
    }
    m_schemamodel->sortSchemas();
}

void ConfigMain::save() {
    uiToModel();
    QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>(this);
    futureWatcher->setFuture(
        QtConcurrent::run<void>(this, &ConfigMain::modelToYaml));
    connect(futureWatcher, &QFutureWatcher<void>::finished, this, [this]() {
        emit changed(false);
        emit saveFinished();
    });
}

void ConfigMain::modelToUi() {
    // set available and enabled input methods
    for (size_t i = 0; i < m_schemamodel->m_schemas.size(); i++) {
        auto &schema = m_schemamodel->m_schemas[i];
        if (schema.active) {
            QStandardItem *active_schema = new QStandardItem(schema.name);
            active_schema->setEditable(false);
            auto qmodel =
                static_cast<QStandardItemModel *>(currentIMView->model());
            qmodel->appendRow(active_schema);
        } else {
            QStandardItem *inactive_schema = new QStandardItem(schema.name);
            inactive_schema->setEditable(false);
            auto qmodel =
                static_cast<QStandardItemModel *>(availIMView->model());
            qmodel->appendRow(inactive_schema);
        }
    }
}

void ConfigMain::updateIMList() {
    auto avail_IMmodel =
        static_cast<QStandardItemModel *>(availIMView->model());
    auto active_IMmodel =
        static_cast<QStandardItemModel *>(currentIMView->model());
    avail_IMmodel->removeRows(0, avail_IMmodel->rowCount());
    active_IMmodel->removeRows(0, active_IMmodel->rowCount());
    for (size_t i = 0; i < m_schemamodel->m_schemas.size(); i++) {
        auto &schema = m_schemamodel->m_schemas[i];
        if (schema.active) {
            QStandardItem *active_schema = new QStandardItem(schema.name);
            active_schema->setEditable(false);
            active_IMmodel->appendRow(active_schema);
        } else {
            QStandardItem *inactive_schema = new QStandardItem(schema.name);
            inactive_schema->setEditable(false);
            avail_IMmodel->appendRow(inactive_schema);
        }
    }
}

void ConfigMain::modelToYaml() {
    // set toggle keys
    std::vector<std::string> toggles;
    std::vector<Keybinding> bindings;
    for (int i = 0; i < m_shortcutmodel->m_entries.size(); i++) {
        auto &entry = m_shortcutmodel->m_entries[i];
        // We need to deal with the toggle menu and number of candidates
        // separately
        if (entry.action == "Toggle menu") {
            toggles.push_back(entry.accept);
        } else if (entry.action == "Number of candidates") {
            config.setInteger("menu/page_size", std::stoi(entry.accept));
        } else {
            bindings.push_back(entry);
        }
    }
    config.setToggleKeys(toggles);
    config.setKeybindings(bindings);

    // Set schemas
    std::vector<std::string> schemaNames;
    schemaNames.reserve(m_schemamodel->m_schemas.size());
    for (int i = 0; i < m_schemamodel->m_schemas.size(); i++) {
        if (m_schemamodel->m_schemas[i].index == 0) {
            break;
        } else {
            schemaNames.push_back(m_schemamodel->m_schemas[i].id.toStdString());
        }
    }
    config.setSchemas(schemaNames);

    config.sync();
    return;
}

void ConfigMain::yamlToModel() {
    m_shortcutmodel->load(config);
    getAvailableSchemas();
}

void ConfigMain::getAvailableSchemas() {
    const char *userPath = RimeGetUserDataDir();
    const char *sysPath = RimeGetSharedDataDir();

    QSet<QString> files;
    for (auto path : {sysPath, userPath}) {
        if (!path) {
            continue;
        }
        QDir dir(path);
        files.unite(QSet<QString>::fromList(dir.entryList(
            QStringList("*.schema.yaml"), QDir::Files | QDir::Readable)));
    }

    auto filesList = files.toList();
    filesList.sort();

    for (const auto &file : filesList) {
        auto schema = FcitxRimeSchema();
        QString fullPath;
        for (auto path : {userPath, sysPath}) {
            QDir dir(path);
            if (dir.exists(file)) {
                fullPath = dir.filePath(file);
                break;
            }
        }
        schema.path = fullPath;
        QFile fd(fullPath);
        if (!fd.open(QIODevice::ReadOnly)) {
            continue;
        }
        auto yamlData = fd.readAll();
        auto name = config.stringFromYAML(yamlData.constData(), "schema/name");
        auto id =
            config.stringFromYAML(yamlData.constData(), "schema/schema_id");
        schema.name = QString::fromStdString(name);
        schema.id = QString::fromStdString(id);
        schema.index = config.schemaIndex(id.data());
        schema.active = static_cast<bool>(schema.index);
        m_schemamodel->m_schemas.push_back(std::move(schema));
    }
    m_schemamodel->sortSchemas();
}

}; // namespace fcitx_rime
