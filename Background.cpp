
#include "Background.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <kstandarddirs.h>
#include <QtDebug>

Background::Background(): tile(true) {

  sourceImage = 0;
  backgroundImage = 0;
  backgroundPixmap = 0;
  isSVG = false;
}

Background::~Background() {
  delete sourceImage;
  delete backgroundImage;
  delete backgroundPixmap;
}

bool Background::load(const QString &file, short width, short height) {
  w=width;
  h=height; 

  //TODO fix this
  //if (file == filename) {
  //	return true;
  //}
  sourceImage = new QImage();
  backgroundImage = new QImage();
  backgroundPixmap = new QPixmap();

  //TODO hardcoded file during transition to SVG rendering
  QString picsPos = "pics/";
  picsPos += "default.bgnd";

  QString newPath = KStandardDirs::locate("appdata", picsPos);

  if (newPath.isEmpty()) {
		qDebug() << "could not find default background";
		return false;
  }

    // try to load it as image
    isSVG = false;
    if( ! sourceImage->load( newPath) ) {
	//maybe SVG??
	//TODO add support for svgz?
	svg.load(newPath);
	if (svg.isValid()) {
		delete sourceImage;
	        sourceImage = new QImage(w, h ,QImage::Format_RGB32);
		//Is it really necessary to fill the buffer?
		sourceImage->fill(0);
	        QPainter p(sourceImage);
	        svg.render(&p);
		isSVG = true;
	    } else {
	        return( false );
	    }
    }

  // Just in case the image is loaded 8 bit
  if (sourceImage->depth() != 32)
    *sourceImage = sourceImage->convertToFormat(QImage::Format_RGB32);  

  // call out to scale/tile the source image into the background image
  //in case of SVG we will be already at the right size
  sourceToBackground();

  filename = file;
	
   return true;
}

// slot used when tile/scale option changes
void Background::scaleModeChanged() {
	sourceToBackground();
}

void Background::sizeChanged(int newW, int newH) {
	if (newW == w && newH == h)
		return;
	w = newW;
	h = newH;
//TODO fix this
  load(filename, w, h);
//was sourceToBackground();
}

void Background::sourceToBackground() {

  // Deallocate the old image and create the new one
  delete backgroundImage;
  // the new version of kmahjongg uses true color images
  // to avoid the old color limitation.
  backgroundImage = new QImage(w, h, QImage::Format_RGB32);

  // Now we decide if we should scale the incoming image
  // or if we tile. First we check for an exact match which
  // should be true for all images created specifically for us.
  if ((sourceImage->width() == w) && (sourceImage->height() == h)) {
    *backgroundImage = *sourceImage;

  // Save a copy of the background as a pixmap for easy and quick
  // blitting.
  *backgroundPixmap = QPixmap::fromImage(*backgroundImage);

    return;
  }
    
  if (tile) {
    // copy new to background wrapping on w and height
    for (int y=0; y<backgroundImage->height(); y++) {
      QRgb *dest = (QRgb *) backgroundImage->scanLine(y);
      QRgb *src = (QRgb *) sourceImage->scanLine(y % sourceImage->height());
      for (int x=0; x< backgroundImage->width(); x++) {
	*dest = *(src + (x % sourceImage->width()));
	dest++;
      }
    }
  } else {
    *backgroundImage = sourceImage->scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // Just incase the image is loaded 8 bit
    if (backgroundImage->depth() != 32)
      *backgroundImage = backgroundImage->convertToFormat(QImage::Format_RGB32);
  }

  // Save a copy of the background as a pixmap for easy and quick
  // blitting.
  *backgroundPixmap = QPixmap::fromImage(*backgroundImage);

  return;
}

