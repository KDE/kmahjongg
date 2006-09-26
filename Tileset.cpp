
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
	scaleddata.ind     = originaldata.ind * ratio;
	scaleddata.qw     = originaldata.qw * ratio;
	scaleddata.qh     = originaldata.qh * ratio;
qDebug() << scaleddata.qw << scaleddata.qh;
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

void Tileset::createTilePixmap(short x, short y, QPixmap& alltiles, QPixmap &dest, bool selected)
{
     if (selected) {
	//get selected version, down 5 rows in the tilemap
	y = y+5;
     }
    dest = alltiles.copy ( QRect(x*scaleddata.w,y*scaleddata.h,scaleddata.w,scaleddata.h) );
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
qDebug() << "could not open file";
      return (false);
    }
    if (!doc.setContent(&tilesetfile)) {
qDebug() << "could not set content";
      tilesetfile.close();
      return (false);
    }
    tilesetfile.close();

    //now see if it is a kdegames mahjongg tileset
    QDomElement root = doc.documentElement();
    if (root.tagName() != "kdegames-tileset") {
qDebug() << "could not find root tagname";
        return (false);
    } else if (root.hasAttribute("version")
                && root.attribute("version") != "1.0") {
qDebug() << "could not find version";
        return (false);
    }

    QDomElement graphics = root.firstChildElement("graphics");
    if (!graphics.isNull()) {
        QString graphName = graphics.firstChildElement("path").text();
    	QString graphPos = "pics/"+graphName;

    	graphicsPath = KStandardDirs::locate("appdata", graphPos);
qDebug() << isSVG;
qDebug() << graphName;
qDebug() << graphPos;
qDebug() << graphicsPath;
	filename = graphicsPath;
	isSVG = (graphics.attribute("type") == "svg");
	originaldata.w      = graphics.firstChildElement("tilewidth").text().toInt();
	originaldata.h      = graphics.firstChildElement("tileheight").text().toInt();
	originaldata.qw  =  graphics.firstChildElement("tilefacewidth").text().toDouble() / 2.0;
	originaldata.qh = graphics.firstChildElement("tilefaceheight").text().toDouble() / 2.0;
	originaldata.ind     =  graphics.firstChildElement("threedindentation").text().toInt();
     	// if (path.isEmpty()) goto exit_gracefully;
     } else {
qDebug() << "null graphics";
	return (false);
     }

    // try to load it as image
    isSVG = false;
    if( ! qiTiles.load( graphicsPath ) ) {
	//SVG?
	svg.load(graphicsPath);
	if (svg.isValid()) {
		filename = tilesetPath;
		isSVG = true;
		//initially setup a medium tilesize, will be resized later
		//TODO get initial aspect ration from tileset specification
		reloadTileset(QSize(42,54));
	    } else {
	        return( false );
	    }
    } else {
	qDebug() << "loaded as png";
    }

    //TODO add support for png
    return( true );
}

// ---------------------------------------------------------
bool Tileset::reloadTileset( QSize newTilesize)
{

    QImage qiTiles;
    QPixmap qiTilespix;
    QString tilesetPath = filename;

    if (QSize(scaleddata.w,scaleddata.h)==newTilesize) return false;

    if( isSVG ) {
	if (svg.isValid()) {
	        qiTiles = QImage(QSize(newTilesize.width()*9, newTilesize.height()*10),QImage::Format_ARGB32_Premultiplied);
		//Fill the buffer, it is unitialized by default
		qiTiles.fill(0);
	        QPainter p(&qiTiles);
	        svg.render(&p);
		updateScaleInfo(newTilesize.width(), newTilesize.height());
	    } else {
	        return( false );
	}
    } else {
    //TODO add support for png
	return false;
    }

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



