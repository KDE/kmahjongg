
####### Defines

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       $(LIB_KFILE)

####### Files

SUBDIRS = pics 

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = kmahjongg.cpp  \
		Tileset.cpp BoardLayout.cpp GameNum.cpp GameTimer.cpp \
		Preferences.cpp Background.cpp \
		PrefsDlg.cpp  Preview.cpp \
		Editor.cpp HighScore.cpp Progress.cpp
		

noinst_HEADERS = kmahjongg.h version.h

METASOURCES = kmahjongg.moc GameNum.moc GameTimer.moc Preview.moc Editor.moc HighScore.moc PrefsDlg.moc Progress.moc

appsdir   = $(kde_appsdir)/Games/Board
apps_DATA = kmahjongg.desktop

KDE_ICON = kmahjongg

EXTRA_DIST = $(apps_DATA) kmahjongg.desktop

messages:
	$(XGETTEXT) -kTRANS $(kmahjongg_SOURCES) -o ../po/kmahjongg.pot

install-data-local:
	rm -rf $(kde_appsdir)/Games/kmahjongg.desktop
