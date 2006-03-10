kde4_header()

add_subdirectory( pics ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kmahjongg_SRCS 
   main.cpp 
   kmahjongg.cpp 
   boardwidget.cpp 
   Tileset.cpp 
   BoardLayout.cpp 
   GameTimer.cpp 
   Background.cpp 
   Preview.cpp 
   Editor.cpp 
   HighScore.cpp )

kde4_automoc(${kmahjongg_SRCS})

kde4_add_ui3_files(kmahjongg_SRCS settings.ui )

kde4_add_kcfg_files(kmahjongg_SRCS prefs.kcfgc )

kde4_add_executable(kmahjongg ${kmahjongg_SRCS})

target_link_libraries(kmahjongg  ${LIB_KIO} kdegames )

install_targets(/bin kmahjongg )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kmahjongg.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES kmahjongg.kcfg )
install_files( ${DATA_INSTALL_DIR}/kmahjongg FILES kmahjonggui.rc )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = pics
#
#INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)
#KDE_ICON = kmahjongg
#
#bin_PROGRAMS = kmahjongg
#kmahjongg_SOURCES = main.cpp kmahjongg.cpp boardwidget.cpp \
#		Tileset.cpp BoardLayout.cpp GameTimer.cpp \
#		Background.cpp Preview.cpp prefs.kcfgc \
#		Editor.cpp HighScore.cpp settings.ui
#kmahjongg_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#kmahjongg_LDADD = $(LIB_KDEGAMES) $(LIB_KFILE)
#kmahjongg_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#METASOURCES = AUTO
#
#xdg_apps_DATA = kmahjongg.desktop
#kde_kcfg_DATA = kmahjongg.kcfg
#
#rcdir = $(kde_datadir)/kmahjongg
#rc_DATA = kmahjonggui.rc
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kmahjongg.pot
#
