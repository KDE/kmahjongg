#ifndef _TILE_SET_H_
#define _TILE_SET_H_

#include <qbitmap.h>
#include <QSvgRenderer>

class Tileset {
   public:
     Tileset();	     
     ~Tileset();	

     bool loadTileset(const QString &filesetPath);
     QRgb *createTile(short x, short y, QRgb *dst, QImage &src , bool selected);
     QRgb *copyTileImage(short tileX, short tileY, QRgb *to, QImage &from);  

     QSize preferredTileSize(QSize boardsize, int horizontalCells, int verticalCells);
     bool reloadTileset( QSize newTilesize);


     QRgb *tile(short tnum);
     QRgb *selectedTile(short tnum);
     short width() {return w;};
     short height() {return h;};
     short shadowSize() {return ss;};
     short size() {return s;};
     short qWidth() {return qw;};
     short qHeight() {return qh;};


     QPixmap *selectedPixmaps(int num) {
		return &(selectedPix[num]);
	};

     QPixmap *unselectedPixmaps(int num) {
		return &(unselectedPix[num]);
	};

  protected:

     enum { maxTiles=45 };
  	void  createPixmap(QRgb *src, QPixmap &dest);
	void initStorage(short tilew, short tileh, short tileshadow, short tileborder,
bool tileisSVG);
  

  private:
    QRgb* tiles;         // Buffer containing all tiles (unselected glyphs)
    QRgb* selectedTiles; // Buffer containing all tiles (selected glyphs)


    // in version 0.5 we have moved ftom using images and calculating
    // masks etc, to using pixmaps and letting the blt do the hard work,
    // in hardware. 
    QPixmap selectedPix[maxTiles]; // selected tiles
    QPixmap unselectedPix[maxTiles]; // selected tiles

    QRgb* selectedFace;  // The tile background face for a selected tile
    QRgb* unselectedFace;// The tile background face for an unselected tile

    short ss;   // left/bottom shadow width
    short bs;   // width of the border around a tile
    short w;    // tile width ( +border +shadow)
    short h;    // tile height ( +border +shadow)
    short qw;   // quarter tile width used in 3d rendering
    short qh;   // quarter tile height used in 3d rendering
    short s;	// buffer size for tile (width*height)

    QString filename;  // cache the last file loaded to save reloading it

    QSvgRenderer svg;
    bool isSVG;
};


#endif


