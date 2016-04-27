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

install(FILES kmahjongg.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES kmahjonggui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kmahjongg)

