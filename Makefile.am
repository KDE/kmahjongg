SUBDIRS = pics

INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)
KDE_ICON = kmahjongg

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = main.cpp kmahjongg.cpp boardwidget.cpp \
		Tileset.cpp BoardLayout.cpp GameTimer.cpp \
		Background.cpp Preview.cpp prefs.kcfgc \
		Editor.cpp HighScore.cpp settings.ui
kmahjongg_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kmahjongg_LDADD = $(LIB_KDEGAMES) $(LIB_KFILE)
kmahjongg_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

METASOURCES = AUTO

xdg_apps_DATA = kmahjongg.desktop
kde_kcfg_DATA = kmahjongg.kcfg

rcdir = $(kde_datadir)/kmahjongg
rc_DATA = kmahjonggui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmahjongg.pot

