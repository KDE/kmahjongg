/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini  <mauricio@tabuleiro.com>

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

#include <stdlib.h>
#include "Tileset.h"
#include <qimage.h>
#include <kstandarddirs.h>
#include <QPainter>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QtDebug>

// ---------------------------------------------------------

Tileset::Tileset()
{
	filename = "";
}

// ---------------------------------------------------------

Tileset::~Tileset() {
}

void Tileset::updateScaleInfo(short tilew, short tileh) 
{
	scaleddata.w = tilew;
	scaleddata.h = tileh;
	double ratio = ((qreal) scaleddata.w) / ((qreal) originaldata.w);
	scaleddata.lvloff     = originaldata.lvloff * ratio;
	scaleddata.fw     = originaldata.fw * ratio;
	scaleddata.fh     = originaldata.fh * ratio;
}

QSize Tileset::preferredTileSize(QSize boardsize, int horizontalCells, int verticalCells)
{
    //calculate our best tile size to fit the boardsize passed to us
    qreal newtilew, newtileh;
    qreal bw = boardsize.width();
    qreal bh = boardsize.height();

    qreal fullh = originaldata.h*verticalCells;
    qreal fullw = originaldata.w*horizontalCells;
    qreal floatw = originaldata.w;
    qreal floath = originaldata.h;

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

/*void Tileset::createTilePixmap(short x, short y, QPixmap& alltiles, QPixmap &dest, bool selected)
{
     if (selected) {
	//get selected version, down 5 rows in the tilemap
	y = y+5;
     }
    dest = alltiles.copy ( QRect(x*scaleddata.w,y*scaleddata.h,scaleddata.w,scaleddata.h) );
} */

bool Tileset::loadDefault()
{
    QString picsPos = "pics/";
    picsPos += "default.tileset";

    QString tilesetPath = KStandardDirs::locate("appdata", picsPos);

    if (tilesetPath.isEmpty()) {
		return false;
    }
    return loadTileset(tilesetPath);
}


// ---------------------------------------------------------
bool Tileset::loadTileset( const QString& tilesetPath)
{

    QImage qiTiles;
    QString graphicsPath;

    if (filename == tilesetPath) {
	return true;
    }

    // verify if it is a valid XML file first and if we can open it
    QDomDocument doc("tileset");
    QFile tilesetfile(tilesetPath);
    if (!tilesetfile.open(QIODevice::ReadOnly)) {
      return (false);
    }
    if (!doc.setContent(&tilesetfile)) {
      tilesetfile.close();
      return (false);
    }
    tilesetfile.close();

    //now see if it is a kdegames mahjongg tileset
    QDomElement root = doc.documentElement();
    if (root.tagName() != "kdegames-tileset") {
        return (false);
    } else if (root.hasAttribute("version")
                && root.attribute("version") != "1.0") {
        return (false);
    }

    QDomElement graphics = root.firstChildElement("graphics");
    if (!graphics.isNull()) {
        QString graphName = graphics.firstChildElement("path").text();
    	QString graphPos = "pics/"+graphName;

    	graphicsPath = KStandardDirs::locate("appdata", graphPos);
	filename = graphicsPath;
	isSVG = (graphics.attribute("type") == "svg");
	originaldata.w      = graphics.firstChildElement("tilewidth").text().toInt();
	originaldata.h      = graphics.firstChildElement("tileheight").text().toInt();
	originaldata.fw  =  graphics.firstChildElement("tilefacewidth").text().toInt();
	originaldata.fh = graphics.firstChildElement("tilefaceheight").text().toInt();
	originaldata.lvloff     =  graphics.firstChildElement("leveloffset").text().toInt();
     	if (graphicsPath.isEmpty()) return (false);
     } else {
	return (false);
     }

    if( isSVG ) {
	//really?
	svg.load(graphicsPath);
	if (svg.isValid()) {
		filename = tilesetPath;
		isSVG = true;
		reloadTileset(QSize(originaldata.w,originaldata.h));
	    } else {
	        return( false );
	    }
    } else {
    //TODO add support for png
	return false;
    }

    return( true );
}

// ---------------------------------------------------------
bool Tileset::reloadTileset( QSize newTilesize)
{

    QImage qiBase;
    QImage qiRend;
    //QPixmap qiTilespix;
    QString tilesetPath = filename;

    if (QSize(scaleddata.w,scaleddata.h)==newTilesize) return false;

    if( isSVG ) {
	if (svg.isValid()) {
		updateScaleInfo(newTilesize.width(), newTilesize.height());
		//render unselected tiles, prepare base image
	        qiBase = QImage(QSize(scaleddata.w, scaleddata.h),QImage::Format_ARGB32_Premultiplied);
		//Fill the buffer, it is uninitialized by default
		qiBase.fill(0);
		//Unselected tiles
		for (short idx=1; idx<=4; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("TILE_%1").arg(idx));
		    unselectedTiles[idx-1] = QPixmap::fromImage(qiRend);
		}
		//Selected tiles
		for (short idx=1; idx<=4; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("TILE_%1_SEL").arg(idx));
		    selectedTiles[idx-1] = QPixmap::fromImage(qiRend);
		}
		//now faces
		qiBase = QImage(QSize(scaleddata.fw, scaleddata.fh),QImage::Format_ARGB32_Premultiplied);
		qiBase.fill(0);
		for (short idx=1; idx<=9; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("CHARACTER_%1").arg(idx));
		    tilefaces[idx+TILE_CHARACTER-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
		for (short idx=1; idx<=9; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("BAMBOO_%1").arg(idx));
		    tilefaces[idx+TILE_BAMBOO-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
		for (short idx=1; idx<=9; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("ROD_%1").arg(idx));
		    tilefaces[idx+TILE_ROD-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
		for (short idx=1; idx<=4; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("SEASON_%1").arg(idx));
		    tilefaces[idx+TILE_SEASON-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
		for (short idx=1; idx<=4; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("WIND_%1").arg(idx));
		    tilefaces[idx+TILE_WIND-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
		for (short idx=1; idx<=3; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("DRAGON_%1").arg(idx));
		    tilefaces[idx+TILE_DRAGON-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
		for (short idx=1; idx<=4; idx++) {
		    qiRend = qiBase;
	            QPainter p(&qiRend);
	            svg.render(&p, QString("FLOWER_%1").arg(idx));
		    tilefaces[idx+TILE_FLOWER-TILE_OFFSET-1] = QPixmap::fromImage(qiRend);
		}
	    } else {
	        return( false );
	}
    } else {
    //TODO add support for png???
	return false;
    }

    // Read in the 9*5 tiles
    /*qiTilespix = QPixmap::fromImage(qiTiles);
    for (short atY=0; atY<5; atY++) {
	for (short atX=0; atX<9; atX++) {
	    int pixNo = atX+(atY*9);
	    createTilePixmap(atX, atY, qiTilespix, unselectedPix[pixNo], false);
	    createTilePixmap(atX, atY, qiTilespix, selectedPix[pixNo], true);
	}
    }*/

    return( true );
}



