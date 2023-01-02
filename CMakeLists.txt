cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "22")
set (RELEASE_SERVICE_VERSION_MINOR "12")
set (RELEASE_SERVICE_VERSION_MICRO "1")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump KMAHJONGG_BASE_VERSION once new features are added
set(KMAHJONGG_BASE_VERSION "0.9")
set(KMAHJONGG_VERSION "${KMAHJONGG_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(kmahjongg VERSION ${KMAHJONGG_VERSION})

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.90.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

if(ENABLE_CLAZY)
    set(CMAKE_CXX_COMPILER clazy)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(FeatureSummary)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Gui Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Completion
    Config
    CoreAddons
    Crash
    DBusAddons
    DocTools
    NewStuff
    XmlGui
)

find_package(KF5KMahjongglib 5.1.0 REQUIRED)
find_package(KF5KDEGames 7.3.0 REQUIRED)

add_definitions(
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050F00
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055600
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
)

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(layouts)
add_subdirectory(doc)

########### install files ###############

install(PROGRAMS org.kde.kmahjongg.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kmahjongg.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ki18n_install(po)
kdoctools_install(po)

########### Display Summary ###############
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
