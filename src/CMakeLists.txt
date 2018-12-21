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
   gameview.cpp
   gamescene.cpp
   selectionanimation.cpp
   demoanimation.cpp
   movelistanimation.cpp
)

ki18n_wrap_ui(kmahjongg_SRCS settings.ui gametype.ui)

ecm_qt_declare_logging_category(kmahjongg_SRCS HEADER kmahjongg_debug.h IDENTIFIER KMAHJONGG_LOG CATEGORY_NAME log_kmahjongg)

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

install(FILES kmahjongg.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES kmahjonggui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kmahjongg)

