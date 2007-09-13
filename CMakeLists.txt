project(kmahjongg)

add_subdirectory( pics )

add_subdirectory( layouts )

include_directories( ${CMAKE_SOURCE_DIR}/libkmahjongg  )


########### next target ###############

set(kmahjongg_SRCS 
   main.cpp 
   kmahjongg.cpp 
   boardwidget.cpp 
   BoardLayout.cpp 
   GameData.cpp 
   Preview.cpp 
   kmahjongglayoutselector.cpp 
   kmahjongglayout.cpp 
   Editor.cpp 
   TileSprite.cpp )

kde4_add_ui_files(kmahjongg_SRCS settings.ui gametype.ui )

kde4_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc )

kde4_add_executable(kmahjongg ${kmahjongg_SRCS})

target_link_libraries(kmahjongg  ${KDE4_KIO_LIBS} kdegames kmahjongglib )

install(TARGETS kmahjongg  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kmahjongg.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmahjongg.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kmahjonggui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmahjongg )

kde4_install_icons( ${ICON_INSTALL_DIR}   )




