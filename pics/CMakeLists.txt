



########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kmahjongg/pics FILES kmahjongg.png kmahjongg_bgnd.png splash.png newnum.xpm )
install_files( ${DATA_INSTALL_DIR}/kmahjongg/pics FILES default.theme pirates.theme )
install_files( ${DATA_INSTALL_DIR}/kmahjongg/pics FILES default.bgnd haze.bgnd pirates.bgnd slate.bgnd  	wood.bgnd )
install_files( ${DATA_INSTALL_DIR}/kmahjongg/pics FILES default.tileset pirates.tileset runes.tileset traditional.tileset )
install_files( ${DATA_INSTALL_DIR}/kmahjongg/pics FILES default.layout cross.layout pirates.layout  	pyramid.layout stax.layout test.layout test2.layout  	tower.layout triangle.layout )




#original Makefile.am contents follow:

#theme_DATA = default.theme pirates.theme
#themedir = $(kde_datadir)/kmahjongg/pics
#
#pics_DATA = kmahjongg.png kmahjongg_bgnd.png splash.png newnum.xpm
#picsdir = $(kde_datadir)/kmahjongg/pics
#
#layout_DATA = default.layout cross.layout pirates.layout \
#	pyramid.layout stax.layout test.layout test2.layout \
#	tower.layout triangle.layout
#layoutdir = $(kde_datadir)/kmahjongg/pics
#
#bgnd_DATA = default.bgnd haze.bgnd pirates.bgnd slate.bgnd \
#	wood.bgnd
#bgnddir = $(kde_datadir)/kmahjongg/pics
#
#tileset_DATA = default.tileset pirates.tileset runes.tileset traditional.tileset
#tilesetdir = $(kde_datadir)/kmahjongg/pics
#
#EXTRA_DIST = $(pics_DATA) $(layout_DATA) $(bgnd_DATA) $(tileset_DATA) $(theme_DATA)
