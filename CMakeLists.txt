project(kmahjongg)

add_subdirectory( layouts )

include_directories( ${CMAKE_SOURCE_DIR}/libkmahjongg  )


########### next target ###############

set(kmahjongg_SRCS 
   main.cpp 
   kmahjongg.cpp 
   boardwidget.cpp 
   BoardLayout.cpp 
   GameData.cpp 
   kmahjongglayoutselector.cpp 
   kmahjongglayout.cpp 
   TileSprite.cpp )

kde4_add_ui_files(kmahjongg_SRCS settings.ui gametype.ui )

kde4_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc )

kde4_add_app_icon(kmahjongg_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-kmahjongg.png")
kde4_add_executable(kmahjongg ${kmahjongg_SRCS})

target_link_libraries(kmahjongg  ${KDE4_KIO_LIBS} kdegames kmahjongglib )

install(TARGETS kmahjongg  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kmahjongg.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmahjongg.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kmahjonggui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmahjongg )

kde4_install_icons( ${ICON_INSTALL_DIR}   )





