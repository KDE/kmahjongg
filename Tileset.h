#ifndef _TILE_SET_H_
#define _TILE_SET_H_


#include <qbitmap.h>

class Tileset {
   public:
     Tileset(bool scaled=false);	     
     ~Tileset();	

     bool loadTileset(const QString &filesetPath, const bool isPreview = false);
     QRgb *createTile(short x, short y, QRgb *dst, QImage &src , QRgb *face);
     QRgb *copyTileImage(short tileX, short tileY, QRgb *to, QImage &from);  

     void setScaled(bool sc) {isScaled = sc; divisor = (sc) ? 2 : 1;}


     QRgb *tile(short tnum);
     QRgb *selectedTile(short tnum);
     short width() {return w/divisor;}
     short height() {return h/divisor;}
     short shadowSize() {return ss/divisor;}
     short size() {return s;}
     short qWidth() {return qw/divisor;}
     short qHeight() {return qh/divisor;}


     QPixmap *selectedPixmaps(int num) {
	if (!isScaled) 
		return &(selectedPix[num]);
	else
		return &(selectedMiniPix[num]);
	}

     QPixmap *unselectedPixmaps(int num) {
	if (!isScaled)
		return &(unselectedPix[num]);
	else
		return &(unselectedMiniPix[num]);
	}

     QPixmap *selectedShadowPixmaps(int num) {
	if (!isScaled) 
		return &(selectedShadowPix[num]);
	else
		return &(selectedShadowMiniPix[num]);
	}

     QPixmap *unselectedShadowPixmaps(int num) {
	if (!isScaled)
		return &(unselectedShadowPix[num]);
	else
		return &(unselectedShadowMiniPix[num]);
	}

  protected:

     enum { maxTiles=45 };
  	void  createPixmap(QRgb *src, QPixmap &dest, bool scale, bool shadow);
  

  private:
    QBitmap maskBits;    // xbm mask for the tile
    QBitmap maskBitsMini;    // xbm mask for the tile
    QRgb* tiles;         // Buffer containing all tiles (unselected glyphs)
    QRgb* selectedTiles; // Buffer containing all tiles (selected glyphs)


    // in version 0.5 we have moved ftom using images and calculating
    // masks etc, to using pixmaps and letting the blt do the hard work,
    // in hardware. 
    QPixmap selectedPix[maxTiles]; // selected tiles
    QPixmap unselectedPix[maxTiles]; // unselected tiles
    QPixmap selectedMiniPix[maxTiles]; // selected tiles
    QPixmap unselectedMiniPix[maxTiles]; // unselected tiles

    QPixmap selectedShadowPix[maxTiles]; // selected tiles as above in shadow
    QPixmap unselectedShadowPix[maxTiles]; // unselected tiles
    QPixmap selectedShadowMiniPix[maxTiles]; // selected tiles
    QPixmap unselectedShadowMiniPix[maxTiles]; // unselected tiles




    QRgb* selectedFace;  // The tile background face for a selected tile
    QRgb* unselectedFace;// The tile background face for an unselected tile

    QRgb  tr;    // transparenct color for tile bg

    short ss;   // left/bottom shadow width
    short bs;   // width of the border around a tile
    short w;    // tile width ( +border +shadow)
    short h;    // tile height ( +border +shadow)
    short qw;   // quarter tile width used in 3d rendering
    short qh;   // quarter tile height used in 3d rendering
    short s;	// buffer size for tile (width*height)

    QString filename;  // cache the last file loaded to save reloading it
    bool isScaled;
    int divisor;
};


#endif


