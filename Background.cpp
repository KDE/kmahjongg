
#include "Background.h"
#include <qimage.h>

Background::Background(): tile(true) {

  sourceImage = 0;
  backgroundImage = 0;
  backgroundPixmap = 0;
  backgroundShadowPixmap = 0;
}

Background::~Background() {
  delete sourceImage;
  delete backgroundImage;
  delete backgroundPixmap;
  delete backgroundShadowPixmap;
}

bool Background::load(const QString &file, short width, short height) {
  w=width;
  h=height; 

  if (file == filename) {
	return true;
  }
  sourceImage = new QImage();
  backgroundImage = new QImage();
  backgroundPixmap = new QPixmap();
  backgroundShadowPixmap = new QPixmap();

  // try to load the image, return on failure 
  if(!sourceImage->load(file ))
    return false;

  // Just in case the image is loaded 8 bit
  if (sourceImage->depth() != 32)
    *sourceImage = sourceImage->convertDepth(32);  

  // call out to scale/tile the source image into the background image
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
	sourceToBackground();
}

void Background::sourceToBackground() {

  // Deallocate the old image and create the new one
  if (!backgroundImage->isNull())
    backgroundImage->reset();

  // the new version of kmahjongg uses true color images
  // to avoid the old color limitation.
  backgroundImage->create(w, h, 32);

  // Now we decide if we should scale the incoming image
  // or if we tile. First we check for an exact match which
  // should be true for all images created specifically for us.
  if ((sourceImage->width() == w) && (sourceImage->height() == h)) {
    *backgroundImage = *sourceImage;
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
    *backgroundImage = sourceImage->smoothScale(w, h);
    // Just incase the image is loaded 8 bit
    if (backgroundImage->depth() != 32)
      *backgroundImage = backgroundImage->convertDepth(32);
  }

  // Save a copy of the background as a pixmap for easy and quick
  // blitting.
  backgroundPixmap->convertFromImage(*backgroundImage);

  QImage tmp;
  tmp.create(backgroundImage->width(), backgroundImage->height(), 32);
  for (int ys=0; ys < tmp.height(); ys++) {
	QRgb *src = (QRgb *) backgroundImage->scanLine(ys);
	QRgb *dst = (QRgb *) tmp.scanLine(ys);
	for (int xs=0; xs < tmp.width(); xs++) {
		*dst=QColor(*src).dark(133).rgb();
		src++;
		dst++;
	}
  }	

  backgroundShadowPixmap->convertFromImage(tmp);

  return;
}

