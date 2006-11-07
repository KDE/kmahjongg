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
#include <QPixmapCache>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QtDebug>

// ---------------------------------------------------------

Tileset::Tileset()
{
	filename = "";
	buildElementIdTable();
}

// ---------------------------------------------------------

Tileset::~Tileset() {
}

void Tileset::updateScaleInfo(short tilew, short tileh) 
{
	scaleddata.w = tilew;
	scaleddata.h = tileh;
	double ratio = ((qreal) scaleddata.w) / ((qreal) originaldata.w);
	scaleddata.lvloff     = (short) (originaldata.lvloff * ratio);
	scaleddata.fw     = (short) (originaldata.fw * ratio);
	scaleddata.fh     = (short) (originaldata.fh * ratio);
}

QSize Tileset::preferredTileSize(QSize boardsize, int horizontalCells, int verticalCells)
{
    //calculate our best tile size to fit the boardsize passed to us
    qreal newtilew, newtileh, aspectratio;
    qreal bw = boardsize.width();
    qreal bh = boardsize.height();

    //use tileface for calculation, with one complete tile in the sum for extra margin
    qreal fullh = (originaldata.fh*verticalCells)+originaldata.h;
    qreal fullw = (originaldata.fw*horizontalCells)+originaldata.w;
    qreal floatw = originaldata.w;
    qreal floath = originaldata.h;

    if ((fullw/fullh)>(bw/bh)) {
        //space will be left on height, use width as limit
	aspectratio = bw/fullw;
    } else {
	aspectratio = bh/fullh;
    }
    newtilew = aspectratio * floatw;
    newtileh = aspectratio * floath;
    return QSize((short)newtilew, (short)newtileh);
}

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
		//invalidate our global cache
		QPixmapCache::clear();

		isSVG = true;
		reloadTileset(QSize(originaldata.w,originaldata.h));
	    } else {
	        return( false );
	    }
    } else {
    //TODO add support for png??
	return false;
    }

    return( true );
}

// ---------------------------------------------------------
bool Tileset::reloadTileset( QSize newTilesize)
{
    QString tilesetPath = filename;

    if (QSize(scaleddata.w,scaleddata.h)==newTilesize) return false;

    if( isSVG ) {
	if (svg.isValid()) {
		updateScaleInfo(newTilesize.width(), newTilesize.height());
		//rendering will be done when needed, automatically using the global cache
	    } else {
	        return( false );
	}
    } else {
    //TODO add support for png???
	return false;
    }

    return( true );
}

void Tileset::buildElementIdTable() {
	//Build a list for faster lookup of element ids, mapped to the enumeration used by GameData and BoardWidget
	//Unselected tiles
	for (short idx=1; idx<=4; idx++) {
		elementIdTable.append(QString("TILE_%1").arg(idx));
	}
	//Selected tiles
	for (short idx=1; idx<=4; idx++) {
		elementIdTable.append(QString("TILE_%1_SEL").arg(idx));
	}
	//now faces
	for (short idx=1; idx<=9; idx++) {
		elementIdTable.append(QString("CHARACTER_%1").arg(idx));
	}
	for (short idx=1; idx<=9; idx++) {
		elementIdTable.append(QString("BAMBOO_%1").arg(idx));
	}
	for (short idx=1; idx<=9; idx++) {
		elementIdTable.append(QString("ROD_%1").arg(idx));
	}
	for (short idx=1; idx<=4; idx++) {
		elementIdTable.append(QString("SEASON_%1").arg(idx));
	}
	for (short idx=1; idx<=4; idx++) {
		elementIdTable.append(QString("WIND_%1").arg(idx));
	}
	for (short idx=1; idx<=3; idx++) {
		elementIdTable.append(QString("DRAGON_%1").arg(idx));
	}
	for (short idx=1; idx<=4; idx++) {
		elementIdTable.append(QString("FLOWER_%1").arg(idx));
	}
}

QString Tileset::pixmapCacheNameFromElementId(QString & elementid) {
	return elementid+QString("W%1H%2").arg(scaleddata.w).arg(scaleddata.h);
}

QPixmap Tileset::renderElement(short width, short height, QString & elementid) {
//qDebug() << "render element" << elementid << width << height;
    QImage qiRend(QSize(width, height),QImage::Format_ARGB32_Premultiplied);
    qiRend.fill(0);

    if (svg.isValid()) {
            QPainter p(&qiRend);
	    svg.render(&p, elementid);
    }
    return QPixmap::fromImage(qiRend);
}

QPixmap Tileset::selectedTile(int num) {
	QPixmap pm;
	QString elemId = elementIdTable.at(num+4);//selected offset in our idtable;
 	if (!QPixmapCache::find(pixmapCacheNameFromElementId(elemId), pm)) {
		//use tile size
     		pm = renderElement(scaleddata.w, scaleddata.h, elemId);
     		QPixmapCache::insert(pixmapCacheNameFromElementId(elemId), pm);
 	}
	return pm;
}

QPixmap Tileset::unselectedTile(int num) {
	QPixmap pm;
	QString elemId = elementIdTable.at(num);
 	if (!QPixmapCache::find(pixmapCacheNameFromElementId(elemId), pm)) {
		//use tile size
     		pm = renderElement(scaleddata.w, scaleddata.h, elemId);
     		QPixmapCache::insert(pixmapCacheNameFromElementId(elemId), pm);
 	}
	return pm;
}

QPixmap Tileset::tileface(int num) {
	QPixmap pm;
	QString elemId = elementIdTable.at(num+8);//tileface offset in our idtable;
 	if (!QPixmapCache::find(pixmapCacheNameFromElementId(elemId), pm)) {
		//use face size
     		pm = renderElement(scaleddata.fw, scaleddata.fh, elemId);
     		QPixmapCache::insert(pixmapCacheNameFromElementId(elemId), pm);
 	}
	return pm;
}


