
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
}


// ---------------------------------------------------------

Tileset::~Tileset() {
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
    return QSize((short)newtilew, (short)newtileh);
}

// --------------------------------------------------------
// create a pixmap for a tile. 

void Tileset::createTilePixmap(short x, short y, QPixmap& alltiles, QPixmap &dest, bool selected)
{
     if (selected) {
	//get selected version, down 5 rows in the tilemap
	y = y+5;
     }
    dest = alltiles.copy ( QRect(x*w,y*h,w,h) );
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
    QPixmap qiTilespix;
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
		//Fill the buffer, it is unitialized by default
		qiTiles.fill(0);
	        QPainter p(&qiTiles);
	        svg.render(&p);
		initStorage(newTilesize.width(), newTilesize.height(), 5, 2, true);
	    } else {
	        return( false );
	}
    }
    //TODO add support for png

    // Read in the 9*5 tiles
    qiTilespix = QPixmap::fromImage(qiTiles);
    for (short atY=0; atY<5; atY++) {
	for (short atX=0; atX<9; atX++) {
	    int pixNo = atX+(atY*9);
	    createTilePixmap(atX, atY, qiTilespix, unselectedPix[pixNo], false);
	    createTilePixmap(atX, atY, qiTilespix, selectedPix[pixNo], true);
	}
    }

    return( true );
}



