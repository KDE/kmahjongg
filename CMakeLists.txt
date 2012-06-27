project(kmahjongg)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames REQUIRED)

	set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
	find_package(LibKMahjongg REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDEGAMES_INCLUDE_DIRS} ${LIBKMAHJONGG_INCLUDE_DIR})
else(NOT COMPILING_KDEGAMES)
	set(LIBKMAHJONGG_LIBRARY kmahjongglib)
endif(NOT COMPILING_KDEGAMES)

add_subdirectory( layouts )

include_directories( ${CMAKE_SOURCE_DIR}/libkmahjongg  )


########### next target ###############

set(kmahjongg_SRCS
   main.cpp
   kmahjongg.cpp
   BoardLayout.cpp
   GameData.cpp
   kmahjongglayoutselector.cpp
   kmahjongglayout.cpp
   TileSprite.cpp
   Editor.cpp
   FrameImage.cpp
   GameItem.cpp
   GameView.cpp
   GameScene.cpp
   SelectionAnimation.cpp)

kde4_add_ui_files(kmahjongg_SRCS settings.ui gametype.ui )

kde4_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc )

kde4_add_app_icon(kmahjongg_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-kmahjongg.png")
kde4_add_executable(kmahjongg ${kmahjongg_SRCS})

target_link_libraries(kmahjongg  ${KDE4_KIO_LIBS} kdegames kdegamesprivate ${LIBKMAHJONGG_LIBRARY} )

install(TARGETS kmahjongg  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS kmahjongg.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmahjongg.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kmahjonggui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmahjongg )

kde4_install_icons( ${ICON_INSTALL_DIR}   )





