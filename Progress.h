
#ifndef PROGRESS_H 
#define  PROGRESS_H

#include <qdialog.h>


class QLabel;
class QPixmap;

class Progress: public QDialog
{
    Q_OBJECT

public:

    Progress 
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    void status(const QString &s);
    
    virtual ~Progress();

protected:
//    void paintEvent( QPaintEvent* pa );
	
private:
    QPixmap *pix;
    QLabel *text;
};

#endif 
