
####### Defines

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       $(LIB_KFILE)

####### Files

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = kmahjongg.cpp static.cpp

noinst_HEADERS = kmahjongg.h version.h

METASOURCES = kmahjongg.moc

appsdir = $(kde_appsdir)/Games
apps_DATA = kmahjongg.kdelnk

pics_DATA = kmahjongg.bmp kmahjongg_bgnd.bmp
picsdir = $(kde_datadir)/kmahjongg/pics

icon_DATA = kmahjongg.xpm
icondir = $(kde_icondir)

EXTRA_DIST = $(apps_DATA) $(pics_DATA) $(icon_DATA) mini-kmahjongg.xpm kmahjongg.xpm kmahjongg.kdelnk kmahjongg.bmp kmahjongg_bgnd.bmp

install-data-local: mini-kmahjongg.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kmahjongg.xpm $(kde_minidir)/kmahjongg.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kmahjongg.xpm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kmahjongg_SOURCES) && mv messages.po ../po/kmahjongg.pot

