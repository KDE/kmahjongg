project(kmahjongg)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

set (QT_MIN_VERSION "5.2.0")

find_package(ECM 1.2.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Gui Svg Qml Quick QuickWidgets)
find_package(KF5 REQUIRED COMPONENTS CoreAddons XmlGui Config KIO DNSSD NewStuff Declarative DocTools KDELibs4Support)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECompilerSettings)
include(KDECMakeSettings)

find_package(KF5KMahjongglib REQUIRED)
find_package(KF5KDEGames 4.9.0 REQUIRED)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DTRANSLATION_DOMAIN="kmahjongg")

add_subdirectory( layouts )
add_subdirectory( doc )

########### next target ###############

set(kmahjongg_SRCS 
   main.cpp 
   kmahjongg.cpp 
   boardwidget.cpp 
   BoardLayout.cpp 
   GameData.cpp 
   kmahjongglayoutselector.cpp 
   kmahjongglayout.cpp 
   TileSprite.cpp
   Editor.cpp
   FrameImage.cpp)

ki18n_wrap_ui(kmahjongg_SRCS settings.ui gametype.ui )

kconfig_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc )

add_executable(kmahjongg ${kmahjongg_SRCS})

target_link_libraries(kmahjongg Qt5::Gui KF5::CoreAddons KF5::KIOWidgets KF5::DNSSD 
                                KF5::XmlGui KF5KDEGames KF5KDEGamesPrivate KF5KMahjongglib )

install(TARGETS kmahjongg  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS kmahjongg.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmahjongg.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kmahjonggui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/kmahjongg )

ecm_install_icons( ICONS
                   128-apps-kmahjongg.png
                   64-apps-kmahjongg.png
                   48-apps-kmahjongg.png
                   32-apps-kmahjongg.png
                   22-apps-kmahjongg.png
                   16-apps-kmahjongg.png
                   sc-apps-kmahjongg.svgz
                   DESTINATION  ${ICON_INSTALL_DIR}
                   THEME hicolor   )

########### Display Summary ###############
feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES)

