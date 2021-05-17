ecm_setup_version(${KMAHJONGG_VERSION}
    VARIABLE_PREFIX KMAHJONGG
    VERSION_HEADER kmahjongg_version.h
)

add_executable(kmahjongg)

target_sources(kmahjongg PRIVATE
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

    kmahjongg.qrc
)

ki18n_wrap_ui(kmahjongg
    settings.ui
    gametype.ui
)

ecm_qt_declare_logging_category(kmahjongg
    HEADER kmahjongg_debug.h
    IDENTIFIER KMAHJONGG_LOG
    CATEGORY_NAME log_kmahjongg
    DESCRIPTION "KMahjongg game"
    EXPORT KMAHJONGG
)

kconfig_add_kcfg_files(kmahjongg prefs.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*-apps-kmahjongg.png")
ecm_add_app_icon(kmahjongg ICONS ${ICONS_SRCS})

target_link_libraries(kmahjongg
    KF5KDEGames
    KF5KDEGamesPrivate
    KF5KMahjongglib
    KF5::CoreAddons
    KF5::XmlGui
    KF5::DBusAddons
    KF5::Crash
    Qt5::Gui
)

install(TARGETS kmahjongg  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES kmahjongg.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})

ecm_qt_install_logging_categories(
    EXPORT KMAHJONGG
    FILE kmahjongg.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
