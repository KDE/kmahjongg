SUBDIRS = pics

INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)
KDE_ICON = kmahjongg

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = main.cpp kmahjongg.cpp boardwidget.cpp \
		Tileset.cpp BoardLayout.cpp GameTimer.cpp \
		Background.cpp \
		Preview.cpp \
		Editor.cpp HighScore.cpp settings.ui
kmahjongg_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kmahjongg_LDADD = $(LIB_KDEGAMES) $(LIB_KFILE)

METASOURCES = AUTO

xdg_apps_DATA = kmahjongg.desktop

EXTRA_DIST = $(xdg_apps_DATA)

rcdir = $(kde_datadir)/kmahjongg
rc_DATA = kmahjonggui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmahjongg.pot

