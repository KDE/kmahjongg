
#include <stdlib.h>
#include "Tileset.h"
#include <qimage.h>


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

Tileset::Tileset(bool scale):
	maskBits(mask_width, mask_height, mask_bits, true),
	maskBitsMini(mini_width, mini_height, mini_bits, true)
{
	isScaled = scale;
	divisor =  (isScaled) ? 2 : 1;

	// set up tile metrics (fixed for now)
	ss     = 4;    // left/bottom shadow width
	bs     = 1;    // tile boarder width
	w      = 40;   // tile width (inc boarder & shadow)
	h      = 56;   // tile height (inc boarder and shadow)
	s      = w*h;  // RGBA's required per tile

	// Allocate memory for the 9*5 tile arrays
    	tiles         = new QRgb [9*5*s];
    	selectedTiles = new QRgb [9*5*s];

    	// allocate memory for single tile storage
    	selectedFace   = new QRgb [s];
    	unselectedFace = new QRgb [s];

	// quarter widths are used as an offset when
	// overlaying tiles in 3 dimensions.
	qw  = ((w-ss)/2) ;
	qh = ((h-ss)/2) ;

	filename = "";
}


// ---------------------------------------------------------

Tileset::~Tileset() {

	// deallocate all memory
    	delete [] tiles;
    	delete [] selectedTiles;
    	delete [] selectedFace;
    	delete [] unselectedFace;
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
		QRgb *det, QImage &allTiles , QRgb *face) {
  QRgb *image ;
  QRgb *to = det;

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

  // calculate the address of the next tile
  return(det+s);
}

// --------------------------------------------------------
// create a pixmap for a tile. Optionally create a scalled
// version, which can be used for mini tile requirements.
// this gives us a small tile for previews and showing
// removed tiles.
void  Tileset::createPixmap(QRgb *src, QPixmap &dest, bool scale, bool shadow)
{

    QImage buff;
    QRgb   *line;

    buff.create(w, h, 32);

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
    	dest.convertFromImage(buff);
    	dest.setMask(maskBits);
    } else {
    	dest.convertFromImage(buff.smoothScale(w/2, h/2));
    	dest.setMask(maskBitsMini);
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


    // try to load it
    if( ! qiTiles.load( tilesetPath) )
        return( false );



    // we deal only with 32 bit images
    if (qiTiles.depth() != 32)
      qiTiles = qiTiles.convertDepth(32);



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


	  nextUnsel = createTile(atX, atY, unsel , qiTiles, unselectedFace);

	  // for the preview dialog we do not create selected tiles
	  if (!isPreview)
	      nextSel = createTile(atX, atY, sel , qiTiles, selectedFace);
	  int pixNo = atX+(atY*9);

	  // for the preview dialog we only create the unselected mini pix
	  if (!isPreview) {
	      createPixmap(sel, selectedPix[pixNo], false, false);
	      createPixmap(unsel, unselectedPix[pixNo], false, false);
	      createPixmap(sel, selectedMiniPix[pixNo], true, false);

	      createPixmap(sel, selectedShadowPix[pixNo], false, true);
	      createPixmap(unsel, unselectedShadowPix[pixNo], false, true);
	      createPixmap(sel, selectedShadowMiniPix[pixNo], true, true);
	      createPixmap(unsel, unselectedShadowMiniPix[pixNo], true, true);
	  }

	  createPixmap(unsel, unselectedMiniPix[pixNo], true, false);
	  sel = nextSel;
	  unsel= nextUnsel;
	}
    }

    filename = tilesetPath;
    return( true );
}


