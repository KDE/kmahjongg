cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "80")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump KMAHJONGG_BASE_VERSION once new features are added
set(KMAHJONGG_BASE_VERSION "0.9")
set(KMAHJONGG_VERSION "${KMAHJONGG_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(kmahjongg VERSION ${KMAHJONGG_VERSION})

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
include(ECMSetupVersion)

find_package(KF5KMahjongglib REQUIRED)
find_package(KF5KDEGames 4.9.0 REQUIRED)

add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_KEYWORDS
    -DQT_STRICT_ITERATORS
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050A00
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x053700
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
)
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
kdoctools_install(po)

########### Display Summary ###############
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

