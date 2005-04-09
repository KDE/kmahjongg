#ifndef _EditorLoadBase_H 
#define _EditorLoadBase_H 

#include <qdialog.h>
#include <qframe.h>   
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kfiledialog.h>

#include "Tileset.h"
#include "BoardLayout.h"
#include "Background.h"

#include "Preview.h"

class Editor: public QDialog
{
    Q_OBJECT

public:


    Editor
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~Editor();



protected slots:
	void topToolbarOption(int w);
        void drawFrameMousePressEvent ( QMouseEvent* );
        void drawFrameMouseMovedEvent ( QMouseEvent *);	

	
protected:
	enum {remove=98, insert=99, move=100};
	void paintEvent( QPaintEvent* pa );
	void setupToolbar();
	void loadBoard();
	bool saveBoard();
	void newBoard();
	void drawBackground(QPixmap *to);
	void drawTiles(QPixmap *to);
	bool testSave();
	void transformPointToPosition(const QPoint &, POSITION &, bool align);
	void drawCursor(POSITION &p, bool visible);
	bool canInsert(POSITION &p);
	void statusChanged();
	QString statusText();
private:
	int mode;
	int numTiles;
	KToolBar *topToolbar;
	FrameImage * drawFrame;
	Tileset tiles;
	BoardLayout theBoard;
	bool clean;
	POSITION currPos;
	QLabel *theLabel;
private:

};

#endif 
