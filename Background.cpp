
/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>

    Kmahjongg is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Background.h"
#include <QImage>
#include <QPixmap>
#include <QPixmapCache>
#include <QPainter>
#include <kstandarddirs.h>
#include <QtDebug>

Background::Background(): tile(true) {
  isSVG = false;
}

Background::~Background() {
}

bool Background::load(const QString &file, short width, short height) {
  w=width;
  h=height; 
qDebug() << "Background loading";
  if (file == filename) {
  	return true;
  }

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
    if( ! sourceImage.load( newPath) ) {
	//maybe SVG??
	//TODO add support for svgz?
	svg.load(newPath);
	if (svg.isValid()) {
		isSVG = true;
		//scale();
	    } else {
	        return( false );
	    }
    }

  // call out to scale/tile the source image into the background image
  //in case of SVG we will be already at the right size
  filename = file;
   return true;
}

// slot used when tile/scale option changes
void Background::scaleModeChanged() {
	//TODO implement tiling
	//sourceToBackground();
	//scale();
}

void Background::sizeChanged(int newW, int newH) {
	if (newW == w && newH == h)
		return;
	w = newW;
	h = newH;
	//scale();
}

void Background::scale() {
qDebug() << "Background scaling";
    if( isSVG) {
	if (svg.isValid()) {
	        sourceImage =  QImage(w, h ,QImage::Format_RGB32);
		sourceImage.fill(0);
	        QPainter p(&sourceImage);
	        svg.render(&p);
	    }
    }
    backgroundPixmap = QPixmap::fromImage(sourceImage);
}

QString Background::pixmapCacheNameFromElementId(QString & elementid) {
	return elementid+QString("W%1H%2").arg(w).arg(h);
}

QPixmap Background::renderBG(short width, short height) {
    QImage qiRend(QSize(width, height),QImage::Format_ARGB32_Premultiplied);
    qiRend.fill(0);

    if (svg.isValid()) {
            QPainter p(&qiRend);
	    svg.render(&p);
    }
    return QPixmap::fromImage(qiRend);
}

QBrush & Background::getBackground() {
 	if (!QPixmapCache::find(pixmapCacheNameFromElementId(filename), backgroundPixmap)) {
     		backgroundPixmap = renderBG(w, h);
     		QPixmapCache::insert(pixmapCacheNameFromElementId(filename), backgroundPixmap);
 	}
	backgroundBrush = QBrush(backgroundPixmap);
        return backgroundBrush;
}

/*
void Background::sourceToBackground() {
  //Sanity check, did we load anything?
  if (!sourceImage) return;

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
}*/

