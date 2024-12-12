cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "25")
set (RELEASE_SERVICE_VERSION_MINOR "03")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump KMAHJONGG_BASE_VERSION once new features are added
set(KMAHJONGG_BASE_VERSION "0.9")
set(KMAHJONGG_VERSION "${KMAHJONGG_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(kmahjongg VERSION ${KMAHJONGG_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

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
include(ECMDeprecationSettings)
include(FeatureSummary)

option(BUILD_DOC "Whether to build the documentation" ON)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    Svg
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    DBusAddons
    I18n
    NewStuff
    XmlGui
)

find_package(KMahjongglib6 6.0.0 REQUIRED)
find_package(KDEGames6 6.0.0 REQUIRED)

if(BUILD_DOC)
    find_package(KF6DocTools ${KF_MIN_VERSION})
    set_package_properties(KF6DocTools PROPERTIES
        DESCRIPTION "Tools to generate documentation"
        TYPE REQUIRED
    )
endif()

ecm_set_disabled_deprecation_versions(
    QT 6.5
    KF 6.0
    KDEGAMES 6.0
)

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(layouts)

########### install files ###############

install(PROGRAMS org.kde.kmahjongg.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kmahjongg.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

if(BUILD_DOC)
    add_subdirectory(doc)
    kdoctools_install(po)
endif()

ki18n_install(po)

########### Display Summary ###############
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
