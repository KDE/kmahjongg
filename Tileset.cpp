
#include <stdlib.h>
#include "Tileset.h"
#include <qimage.h>
#include <kstandarddirs.h>
#include <QSvgRenderer>
#include <QPainter>
#include <QtDebug>


#define mini_width 20
#define mini_height 28
static unsigned char mini_bits[] = {
  0xfc, 0xff, 0x0f, 0xfe, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x07, 0xff, 0xff, 0x03,
  };

#define mask_width 40
#define mask_height 56
static unsigned char mask_bits[] = {
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff,
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x7f, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff,
  0xff, 0xff, 0xff, 0x0f, };


// ---------------------------------------------------------

Tileset::Tileset(bool scale)
{
	maskBits = QBitmap::fromData(QSize(mask_width,mask_height), mask_bits);
	maskBitsMini = QBitmap::fromData(QSize(mini_width, mini_height), mini_bits);

	isScaled = scale;
	divisor =  (isScaled) ? 2 : 1;

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
		QRgb *det, QImage &allTiles , QRgb *face, bool SVGuseselected) {
  QRgb *image ;
  QRgb *to = det;

  if (isSVG) {
     if (SVGuseselected) {
	//get selected version, down 5 lines in the pixmap
	//we want to preserve our nice antialiased rendering
	y = y+5;
     }
     copyTileImage(x, y , det, allTiles);
  } else {

  // Alloc the space
  image = new QRgb[s];

  // copy in the background
  memcpy(to, face, s*sizeof(QRgb));

  // get the tile gylph
  copyTileImage(x, y , image, allTiles);

  // copy the image over the background using the
  // top left colour as the transparency. We step over
  // the shadow and the boarder

  QRgb* src =  image+                        // image
    ss+             // past the left shadow
    bs+             // then the tile border
    (bs  * w);      // then step past the top border


  to += (((ss+bs))+(bs*w));


  QRgb trans = *src;

  // start after the top border rows and go through all rows
  for( short YP=0; YP < h-ss - (2*bs); YP++) {
    // start after the shadow and border and iterate over x
    for (short xpos=0; xpos < w-ss -(2*bs) ; xpos++) {
      // skip transparent pixels
      if (*src != trans)
	*to = *src;
      src++;
      to++;
    }
    // step over the border to get to the next row
    src  += ss + (2 * bs);
    to += ss + (2 * bs);
    }

  // Free allocated space
  delete [] image;
  }
  // calculate the address of the next tile
  return(det+s);
}

// --------------------------------------------------------
// create a pixmap for a tile. Optionally create a scaled
// version, which can be used for mini tile requirements.
// this gives us a small tile for previews and showing
// removed tiles.
void  Tileset::createPixmap(QRgb *src, QPixmap &dest, bool scale, bool shadow)
{

    QImage buff(w, h, QImage::Format_ARGB32_Premultiplied);
    QRgb   *line;

    for (int y=0; y<h; y++) {
	line = (QRgb *) buff.scanLine(y);
	memcpy( line, src, w*sizeof(QRgb));

	if (shadow) {
	  for (int spos=0; spos <w; spos++) {
	    line[spos] = QColor(line[spos]).dark(133).rgb();
	  }
	}

	src += w;
    }

    // create the pixmap and initialise the drawing mask
    if (!scale) {
    	dest = QPixmap::fromImage(buff);
    	if (!isSVG) dest.setMask(maskBits);
    } else {
    	dest = QPixmap::fromImage(buff.scaled(w/2, h/2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    	if (!isSVG) dest.setMask(maskBitsMini);
    }

}


// ---------------------------------------------------------
bool Tileset::loadTileset( const QString& tilesetPath, const bool isPreview)
{

    QImage qiTiles;
    QRgb *unsel;
    QRgb *sel;
    QRgb *nextSel=0;
    QRgb *nextUnsel=0;

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
	QSvgRenderer svg(newPath);
	if (svg.isValid()) {
	        qiTiles = QImage(svg.defaultSize(),QImage::Format_ARGB32_Premultiplied);
	        QPainter p(&qiTiles);
	        svg.render(&p);
		isSVG = true;
		initStorage(86, 106, 10, 2, true);
	    } else {
	        return( false );
	    }
    }


    if (!isSVG) {
	//legacy pixmap tile options
	initStorage(40, 56, 4, 1, false);
    }

    // we deal only with 32 bit images (does not apply to SVG, only legacy)
    if (qiTiles.depth() != 32)
      qiTiles = qiTiles.convertToFormat(QImage::Format_RGB32);



    // Read in the unselected and selected tile backgrounds
    copyTileImage(7, 4 , unselectedFace, qiTiles);
    copyTileImage(8, 4, selectedFace, qiTiles);



    // Read in the 9*5 tiles. Each tile is overlayed onto
    // the selected and unselected tile backgrounds and
    // stored.
    sel = selectedTiles;
    unsel = tiles;
    for (short atY=0; atY<5; atY++) {
	for (short atX=0; atX<9; atX++) {


	  nextUnsel = createTile(atX, atY, unsel , qiTiles, unselectedFace, false);

	  // for the preview dialog we do not create selected tiles
	  if (!isPreview)
	      nextSel = createTile(atX, atY, sel , qiTiles, selectedFace, true);
	  int pixNo = atX+(atY*9);

	  // for the preview dialog we only create the unselected mini pix
	  if (!isPreview) {
	      createPixmap(sel, selectedPix[pixNo], false, false);
	      createPixmap(unsel, unselectedPix[pixNo], false, false);
	      createPixmap(sel, selectedMiniPix[pixNo], true, false);

	  }

	  createPixmap(unsel, unselectedMiniPix[pixNo], true, false);
	  sel = nextSel;
	  unsel= nextUnsel;
	}
    }

    filename = newPath;
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
	QSvgRenderer svg(newPath);
	if (svg.isValid()) {
	        qiTiles = QImage(QSize(newTilesize.width()*9, newTilesize.height()*10),QImage::Format_ARGB32_Premultiplied);
	        QPainter p(&qiTiles);
	        svg.render(&p);
		isSVG = true;
		initStorage(newTilesize.width(), newTilesize.height(), 5, 2, true);
	    } else {
	        return( false );
	    }
    }


    // Read in the unselected and selected tile backgrounds
    copyTileImage(7, 4 , unselectedFace, qiTiles);
    copyTileImage(8, 4, selectedFace, qiTiles);



    // Read in the 9*5 tiles. Each tile is overlayed onto
    // the selected and unselected tile backgrounds and
    // stored.
    sel = selectedTiles;
    unsel = tiles;
    for (short atY=0; atY<5; atY++) {
	for (short atX=0; atX<9; atX++) {


	  nextUnsel = createTile(atX, atY, unsel , qiTiles, unselectedFace, false);


	  nextSel = createTile(atX, atY, sel , qiTiles, selectedFace, true);
	  int pixNo = atX+(atY*9);

	      createPixmap(sel, selectedPix[pixNo], false, false);
	      createPixmap(unsel, unselectedPix[pixNo], false, false);
	      createPixmap(sel, selectedMiniPix[pixNo], true, false);

	  createPixmap(unsel, unselectedMiniPix[pixNo], true, false);
	  sel = nextSel;
	  unsel= nextUnsel;
	}
    }

    return( true );
}



