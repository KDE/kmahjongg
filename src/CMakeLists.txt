set(kmahjongg_SRCS
   main.cpp
   kmahjongg_debug.cpp
   kmahjongg.cpp
   boardlayout.cpp
   gamedata.cpp
   kmahjongglayoutselector.cpp
   kmahjongglayout.cpp
   editor.cpp
   frameimage.cpp
   gameitem.cpp
   gamebackground.cpp
   gameremovedtiles.cpp
   gameview.cpp
   gamescene.cpp
   selectionanimation.cpp
   demoanimation.cpp
   movelistanimation.cpp
)

ecm_setup_version(${KMAHJONGG_VERSION}
    VARIABLE_PREFIX KMAHJONGG
    VERSION_HEADER kmahjongg_version.h
)

ki18n_wrap_ui(kmahjongg_SRCS settings.ui gametype.ui)

ecm_qt_declare_logging_category(kmahjongg_SRCS
    HEADER kmahjongg_debug.h
    IDENTIFIER KMAHJONGG_LOG
    CATEGORY_NAME log_kmahjongg
    DESCRIPTION "KMahjongg game"
    EXPORT KMAHJONGG
)

kconfig_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*-apps-kmahjongg.png")
ecm_add_app_icon(kmahjongg_SRCS ICONS ${ICONS_SRCS})
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

install(FILES kmahjongg.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES kmahjonggui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kmahjongg)

ecm_qt_install_logging_categories(
    EXPORT KMAHJONGG
    FILE kmahjongg.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
