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

qt5_wrap_ui(RIME_CONFIG_SRCS addshortcutdialog.ui configmain.ui)
add_libraty(fcitx5-rime-config
  MODULE ${RIME_CONFIG_SRCS})

add_definitions(-DQT_NO_KEYWORDS -std=c++11)

target_include_directories(fcitx5-rime-config PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(fcitx5-rime-config PROPERTIES
  LINK_FLAGS "-Wl,--no-undefined"
  AUTOMOC TRUE
  AUTOUIC TRUE
  AUTOUIC_OPTIONS "-tr=fcitx::tr2fcitx;--include=fcitxqti18nhelper.h")

target_link_libraries(fcitx5-rime-config
  Qt5::Concurrent
  Qt5::Core
  Qt5::Widgets
  Fcitx5Qt5::WidgetsAddons
  PkgConfig::Rime)

install(TARGETS fcitx5-rime-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/fcitx5/qt5)
