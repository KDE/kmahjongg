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

    bool load(const char *file, short width, short height);
    void sizeChanged(int newW, int newH);
    void scaleModeChanged(void);
    QPixmap *getBackground(void) {return backgroundPixmap;};
    QPixmap *getShadowBackground(void) {return backgroundShadowPixmap;};
 private:
    void sourceToBackground(void);

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
