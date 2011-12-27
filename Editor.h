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
#include "kmahjonggbackground.h"
#include "BoardLayout.h"

#include "FrameImage.h"


class QLabel;
class KActionCollection;
class QLabel;

/**
 * @short This class implements
 * 
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class Editor: public KDialog
{
    Q_OBJECT

public:
    /**
     * Constructor Description @param parent */
    explicit Editor ( QWidget* parent = 0);
    /**
     * Default Destructor */
    virtual ~Editor();

protected slots:
    /**
     * Slot Description */
    void drawFrameMousePressEvent ( QMouseEvent* );
    /**
     * Slot Description */
    void drawFrameMouseMovedEvent ( QMouseEvent *);
    /**
     * Slot Description */
	void loadBoard();
    /**
     * Slot Description 
     * @return @c true if ...
     * @return @c false if ...
     */
	bool saveBoard();
    /**
     * Slot Description */
	void newBoard();
    /**
     * Slot Description */
    void slotShiftLeft();
    /**
     * Slot Description */
    void slotShiftRight();
    /**
     * Slot Description */
    void slotShiftUp();
    /**
    * Slot Description */
    void slotShiftDown();
    /**
     * Slot Description */
    void slotModeChanged(QAction*);
    /**
     * Slot Description */
protected:
	/** @short Describe enum
     */
    enum {
        remove=98, /**< describe*/
        insert=99, /**< describe*/
        move=100   /**< describe*/
    };
    /**
     * Method Description */
	void resizeEvent ( QResizeEvent * event );
    /**
     * Method Description */
	void paintEvent( QPaintEvent* pa );
    /**
     * Method Description */
	void setupToolbar();
    /**
     * Method Description */
	void drawBackground(QPixmap *to);
    /**
     * Method Description @param to destination QPixmap to draw to */
	void drawTiles(QPixmap *to);
    /**
     * Method Description
     * 
     * @return @c true if
     * @return @c false if
     */
	bool testSave();
    /**
     * Method Description */
	void transformPointToPosition(const QPoint &, POSITION &, bool align);
    /**
     * Method Description 
       @param p @ref pos
       @param visible 
     */
	void drawCursor(POSITION &p, bool visible);
    /**
     * Method Description
     * 
     * @param p @ref pos
     * @return @c true if
     * @return @c false if
     */
	bool canInsert(POSITION &p);
    /**
     * Method Description */
	void statusChanged();
    /**
     * Method Description @return status description */
	QString statusText();
private:
    int borderLeft;
    int borderTop;
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
