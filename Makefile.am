
####### Defines

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       $(LIB_KFILE)

####### Files

SUBDIRS = pics

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = kmahjongg.cpp static.cpp

noinst_HEADERS = kmahjongg.h version.h

METASOURCES = kmahjongg.moc

appsdir = $(kde_appsdir)/Games
apps_DATA = kmahjongg.desktop

KDE_ICON = kmahjongg

EXTRA_DIST = $(apps_DATA) kmahjongg.desktop

messages:
	$(XGETTEXT) -C -kTRANS -ki18n -x $(includedir)/kde.pot $(kmahjongg_SOURCES) && mv messages.po ../po/kmahjongg.pot
