
PICSDIR = $(kde_datadir)/kmahjongg/pics
DOCSDIR = $(kde_htmldir)/en/kmahjongg
APPSDIR = $(kde_appsdir)/Games
ICONDIR = $(kde_icondir)
MINIDIR = $(kde_minidir)

####### Defines

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =             $(LIB_KDEUI)

####### Files

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = kmahjongg.cpp static.cpp

noinst_HEADERS = kmahjongg.h version.h

SRCMETA	=	kmahjongg.moc
DISTCLEANFILES = $(SRCMETA)

docs_DATA    =	index.html index-1.html index-2.html logotp3.gif
docsdir = $(kde_htmldir)/en/kmahjongg

appsdir = $(kde_appsdir)/Games
apps_DATA = kmahjongg.kdelnk

pics_DATA = kmahjongg.bmp kmahjongg_bgnd.bmp
picsdir = $(kde_datadir)/kmahjongg/pics

icon_DATA = kmahjongg.xpm
icondir = $(kde_icondir)

EXTRA_DIST = $(docs_DATA) $(apps_DATA) $(pics_DATA) $(icon_DATA) mini-kmahjongg.xpm kmahjongg.xpm kmahjongg.kdelnk kmahjongg.bmp kmahjongg_bgnd.bmp

install-data-local: mini-kmahjongg.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kmahjongg.xpm $(kde_minidir)/kmahjongg.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kmahjongg.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kmahjongg_SOURCES) && mv messages.po ../po/kmahjongg.pot

kmahjongg.cpp:kmahjongg.moc
kmahjongg.moc: kmahjongg.h
	$(MOC) kmahjongg.h -o kmahjongg.moc



