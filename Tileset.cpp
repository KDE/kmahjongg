
#include <stdlib.h>
#include "Tileset.h"
#include <qimage.h>
#include <kstandarddirs.h>
#include <QPainter>
#include <QtDebug>

// ---------------------------------------------------------

Tileset::Tileset()
{
	filename = "";
	tiles = 0;
    	selectedTiles = 0;
    	selectedFace   = 0;
    	unselectedFace = 0;

	//init storage with default values for KMahjongg classic pixmap tiles
	initStorage(40, 56, 4, 1, false);

}


// ---------------------------------------------------------

Tileset::~Tileset() {

	// deallocate all memory
    	delete [] tiles;
    	delete [] selectedTiles;
    	delete [] selectedFace;
    	delete [] unselectedFace;
}

void Tileset::initStorage(short tilew, short tileh, short tileshadow, short tileborder,
bool tileisSVG) 
{
	//TODO read real metrics from external XML file or SVG metadata
	isSVG = tileisSVG;
	ss     = tileshadow;    // left/bottom shadow width //actually used for 3d indent
	bs     = tileborder;    // tile boarder width
	w      = tilew;   // tile width (inc boarder & shadow)
	h      = tileh;   // tile height (inc boarder and shadow)
	s      = w*h;  // RGBA's required per tile
	// quarter widths are used as an offset when
	// overlaying tiles in 3 dimensions.
	qw  = 7*w/8/2;//((w-ss)/2) ; 
	qh = 7*h/8/2;//((h-ss)/2);

	if (tiles) delete [] tiles;
    	if (selectedTiles) delete [] selectedTiles;
    	if (selectedFace) delete [] selectedFace;
    	if (unselectedFace) delete [] unselectedFace;

	// Allocate memory for the 9*5 tile arrays
    	tiles         = new QRgb [9*5*s];
    	selectedTiles = new QRgb [9*5*s];

    	// allocate memory for single tile storage
    	selectedFace   = new QRgb [s];
    	unselectedFace = new QRgb [s];

}

QSize Tileset::preferredTileSize(QSize boardsize, int horizontalCells, int verticalCells)
{
    //calculate our best tile size to fit the boardsize passed to us
    qreal newtilew, newtileh;
    qreal bw = boardsize.width();
    qreal bh = boardsize.height();
//TODO use tileset original aspect ration for this calculation, origH and origW
    qreal fullh = h*verticalCells;
    qreal fullw = w*horizontalCells;
    qreal floatw = w;
    qreal floath = h;

    if ((fullw/fullh)>(bw/bh)) {
        //space will be left on height, use width as limit
	newtilew = bw/(qreal) horizontalCells;
	newtileh = (floath * newtilew) / floatw;
    } else {
	newtileh = bh/(qreal) verticalCells;
	newtilew = (floatw * newtileh) / floath;
    }
    return QSize(newtilew, newtileh);
}

// ---------------------------------------------------------
// copy a tile from a qimage into a linear array of bytes. This
// method returns the address of the byte after the copied image
// and can be used to fill a larger array of tiles.

QRgb *Tileset::copyTileImage(short tileX, short tileY, QRgb *to, QImage &from) {
    QRgb *dest = to;
    QRgb *src;

    src = (QRgb *) from.scanLine(tileY * h)
             +(tileX * w);
    for (short pos=0; pos < h; pos++) {
	memcpy(dest, src, w*sizeof(QRgb));
	dest+=w;
	src += from.width();
    }
    return(dest);
}

// ----------------------------------------------------------
// Create a tile. Take a specified tile background a tile face
// (specified as an x,y coord) and a destination buffer (location
// in which is calculated from the x,y) and copy in the
// tile background, overlaying the foreground with transparency
// (the foregrounds top/left pixel is taken as the transparent
// color).


QRgb *Tileset::createTile(short x, short y,
		QRgb *det, QImage &allTiles , bool selected) {

  if (isSVG) {
     if (selected) {
	//get selected version, down 5 lines in the pixmap
	//we want to preserve our nice antialiased rendering
	y = y+5;
     }
     copyTileImage(x, y , det, allTiles);
  } 
  // calculate the address of the next tile
  return(det+s);
}

// --------------------------------------------------------
// create a pixmap for a tile. Optionally create a scaled
// version, which can be used for mini tile requirements.
// this gives us a small tile for previews and showing
// removed tiles.
void  Tileset::createPixmap(QRgb *src, QPixmap &dest)
{

    QImage buff(w, h, QImage::Format_ARGB32_Premultiplied);
    QRgb   *line;

    for (int y=0; y<h; y++) {
	line = (QRgb *) buff.scanLine(y);
	memcpy( line, src, w*sizeof(QRgb));

	src += w;
    }

    dest = QPixmap::fromImage(buff);
}


// ---------------------------------------------------------
bool Tileset::loadTileset( const QString& tilesetPath)
{

    QImage qiTiles;

    if (filename == tilesetPath) {
	return true;
    }

    //QString newPath = tilesetPath;
   //TODO hardcoded file during transition to SVG rendering
    QString picsPos = "pics/";
    picsPos += "default.tileset";

    QString newPath = KStandardDirs::locate("appdata", picsPos);

    if (newPath.isEmpty()) {
		qDebug() << "could not find tileset";
		return false;
    }

    // try to load it as image
    isSVG = false;
    if( ! qiTiles.load( newPath) ) {
	//SVG?
	//TODO add support for svgz?
	svg.load(newPath);
	if (svg.isValid()) {
		filename = newPath;
		isSVG = true;
		//initially setup a medium tilesize, will be resized later
		//TODO get initial aspect ration from tileset specification
		reloadTileset(QSize(42,54));
	    } else {
	        return( false );
	    }
    }
    //TODO add support for png
    return( true );
}

// ---------------------------------------------------------
bool Tileset::reloadTileset( QSize newTilesize)
{

    QImage qiTiles;
    QRgb *unsel;
    QRgb *sel;
    QRgb *nextSel=0;
    QRgb *nextUnsel=0;

    QString newPath = filename;

    if (QSize(w,h)==newTilesize) return false;

    // try to load it as image
    isSVG = false;
    if( ! qiTiles.load( newPath) ) {
	//maybe SVG??
	//TODO add support for svgz?
	if (svg.isValid()) {
		isSVG = true;
	        qiTiles = QImage(QSize(newTilesize.width()*9, newTilesize.height()*10),QImage::Format_ARGB32_Premultiplied);
	        QPainter p(&qiTiles);
	        svg.render(&p);
		initStorage(newTilesize.width(), newTilesize.height(), 5, 2, true);
	    } else {
	        return( false );
	}
    }
    //TODO add support for png

    // Read in the 9*5 tiles
    sel = selectedTiles;
    unsel = tiles;
    for (short atY=0; atY<5; atY++) {
	for (short atX=0; atX<9; atX++) {
	  nextUnsel = createTile(atX, atY, unsel , qiTiles, false);
	  nextSel = createTile(atX, atY, sel , qiTiles, true);
	  int pixNo = atX+(atY*9);

	  createPixmap(sel, selectedPix[pixNo]);
          createPixmap(unsel, unselectedPix[pixNo]);

	  sel = nextSel;
	  unsel= nextUnsel;
	}
    }

    return( true );
}



