SUBDIRS = pics

INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD =  $(LIB_KDEGAMES) $(LIB_KFILE)
KDE_ICON = kmahjongg

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = kmahjongg.cpp  \
		Tileset.cpp BoardLayout.cpp GameTimer.cpp \
		Preferences.cpp Background.cpp \
		PrefsDlg.cpp  Preview.cpp \
		Editor.cpp HighScore.cpp Progress.cpp
METASOURCES = kmahjongg.moc GameTimer.moc Preview.moc Editor.moc \
        HighScore.moc PrefsDlg.moc Progress.moc

appsdir   = $(kde_appsdir)/Games/Board
apps_DATA = kmahjongg.desktop

EXTRA_DIST = $(apps_DATA)

rcdir = $(kde_datadir)/kmahjongg
rc_DATA = kmahjonggui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmahjongg.pot

install-data-local:
	rm -rf $(DESTDIR)$(kde_appsdir)/Games/kmahjongg.desktop
