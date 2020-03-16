set(RIME_CONFIG_SRCS
  addshortcutdialog.cpp
  configmain.cpp
  configparser.cpp
  keyseq.cpp
  main.cpp
  schemamodel.cpp
  shortcutmodel.cpp)

if(NOT ENABLE_QT)
  return()
endif()

add_definitions(-DQT_NO_KEYWORDS -std=c++11)

qt5_wrap_ui(RIME_CONFIG_SRCS addshortcutdialog.ui configmain.ui)

target_link_libraries(fcitx5-rime-config
  Qt5::Concurrent
  Qt5::Core
  Qt5::Widgets
  Fcitx5Qt5::WidgetsAddons
  PkgConfig::Rime)

install(TARGETS fcitx5-rime-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/fcitx5/qt5)
