project(kmahjongg)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.2.0")
set(KF5_MIN_VERSION "5.15.0")

find_package(ECM 1.2.0 REQUIRED NO_MODULE)

if(ENABLE_CLAZY)
    set(CMAKE_CXX_COMPILER clazy)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Gui Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    DBusAddons
    Declarative
    DocTools
    NewStuff
    XmlGui
)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(KF5KMahjongglib REQUIRED)
find_package(KF5KDEGames 4.9.0 REQUIRED)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory(icons)
add_subdirectory(layouts)
add_subdirectory(doc)

########### next target ###############

set(kmahjongg_SRCS
   main.cpp
   kmahjongg_debug.cpp
   kmahjongg.cpp
   BoardLayout.cpp
   GameData.cpp
   kmahjongglayoutselector.cpp
   kmahjongglayout.cpp
   Editor.cpp
   FrameImage.cpp
   GameItem.cpp
   GameView.cpp
   GameScene.cpp
   SelectionAnimation.cpp
   DemoAnimation.cpp
   MoveListAnimation.cpp
)

ki18n_wrap_ui(kmahjongg_SRCS settings.ui gametype.ui)

kconfig_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc)

add_executable(kmahjongg ${kmahjongg_SRCS})

target_link_libraries(kmahjongg
    Qt5::Gui
    KF5::CoreAddons
    KF5::XmlGui
    KF5::DBusAddons
    KF5::Crash
    KF5KDEGames
    KF5KDEGamesPrivate
    KF5KMahjongglib
)

install(TARGETS kmahjongg  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(PROGRAMS org.kde.kmahjongg.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES kmahjongg.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES kmahjonggui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kmahjongg)

########### Display Summary ###############
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

