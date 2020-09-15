cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(kmahjongg)

set(QT_MIN_VERSION "5.10.0")
set(KF5_MIN_VERSION "5.55.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

if(ENABLE_CLAZY)
    set(CMAKE_CXX_COMPILER clazy)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

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

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)


find_package(KF5KMahjongglib REQUIRED)
find_package(KF5KDEGames 4.9.0 REQUIRED)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
if (${KF5Config_VERSION} STRGREATER "5.56.0")
    add_definitions(-DQT_NO_FOREACH)
endif()

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(layouts)
add_subdirectory(doc)

########### next target ###############


########### install files ###############

install(PROGRAMS org.kde.kmahjongg.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kmahjongg.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
if (${ECM_VERSION} STRGREATER "5.58.0")
    install(FILES kmahjongg.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
else()
    install(FILES kmahjongg.categories  DESTINATION ${KDE_INSTALL_CONFDIR})
endif()

ki18n_install(po)

########### Display Summary ###############
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

