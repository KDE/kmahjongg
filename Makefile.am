
PICSDIR = $(kde_datadir)/kmahjongg/pics
DOCSDIR = $(kde_htmldir)/en/kmahjongg
APPSDIR = $(kde_appsdir)/Games
ICONDIR = $(kde_icondir)
MINIDIR = $(kde_minidir)

####### Defines

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       -lkdeui -lkdecore -lqt -lX11 -lXext $(LIBSOCKET)

####### Files

bin_PROGRAMS = kmahjongg
kmahjongg_SOURCES = kmahjongg.cpp static.cpp

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

install-local-data: mini-kmahjongg.xpm
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kmahjongg.xpm $(kde_minidir)

uninstall:
	-rm -f $(kde_minidir)/kmahjongg.xpm

messages:
	@XGETTEXT@ -ktranslate -ki18n -x../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kmahjongg.pot

kmahjongg.cpp:kmahjongg.moc
kmahjongg.moc: kmahjongg.h
	$(MOC) kmahjongg.h -o kmahjongg.moc



