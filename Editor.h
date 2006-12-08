/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>

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

#ifndef _EditorLoadBase_H 
#define _EditorLoadBase_H 

#include <kdialog.h>
#include <qframe.h>   
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kfiledialog.h>

#include "kmahjonggtileset.h"
#include "BoardLayout.h"
#include "Background.h"

#include "Preview.h"


class KActionCollection;

class Editor: public KDialog
{
    Q_OBJECT

public:
    Editor ( QWidget* parent = 0);

    virtual ~Editor();

protected slots:
        void drawFrameMousePressEvent ( QMouseEvent* );
        void drawFrameMouseMovedEvent ( QMouseEvent *);	
	void loadBoard();
	bool saveBoard();
	void newBoard();
        void slotShiftLeft();
        void slotShiftRight();
        void slotShiftUp();
        void slotShiftDown();
        void slotModeChanged(QAction*);
protected:
	enum {remove=98, insert=99, move=100};
	void resizeEvent ( QResizeEvent * event );
	void paintEvent( QPaintEvent* pa );
	void setupToolbar();
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
        KActionCollection* actionCollection;
	FrameImage * drawFrame;
	KMahjonggTileset tiles;
	BoardLayout theBoard;
	bool clean;
	POSITION currPos;
	QLabel *theLabel;
};

#endif 
