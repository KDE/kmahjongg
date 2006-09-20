#ifndef _BACKGROUND_H
#define _BACKGROUND_H
#include <QString>
#include "ksvgrenderer.h"

class QPixmap;
class QImage;


class Background  
{


  public:
    Background();
    ~Background();
    bool tile;
    bool isSVG;
    
    bool load(const QString &file, short width, short height);
    void sizeChanged(int newW, int newH);
    void scaleModeChanged();
    QPixmap *getBackground() {return backgroundPixmap;};
 private:
    void sourceToBackground();

    int tileMode;    // scale background = 0, tile = 1
    QImage *backgroundImage;
    QImage *sourceImage;
    QPixmap *backgroundPixmap;
    QString filename;
    short w;
    short h;

    KSvgRenderer svg;
};

#endif
