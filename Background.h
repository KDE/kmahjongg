#ifndef _BACKGROUND_H
#define _BACKGROUND_H
#include <qstring.h>

class QPixmap;
class QImage;


class Background  
{


  public:
    Background();
    ~Background();
    bool tile;
    
    bool load(const QString &file, short width, short height);
    void sizeChanged(int newW, int newH);
    void scaleModeChanged();
    QPixmap *getBackground() {return backgroundPixmap;}
    QPixmap *getShadowBackground() {return backgroundShadowPixmap;}
 private:
    void sourceToBackground();

    int tileMode;    // scale background = 0, tile = 1
    QImage *backgroundImage;
    QImage *sourceImage;
    QPixmap *backgroundPixmap;
    QPixmap *backgroundShadowPixmap;
    QString filename;
    short w;
    short h;
};

#endif
