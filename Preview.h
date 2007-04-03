/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>

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

#ifndef _PreviewLoadBase_H
#define _PreviewLoadBase_H

#include <kdialog.h>

#include <qframe.h>
#include "kgamecanvas.h"

#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "BoardLayout.h"

class QComboBox;
class QPixmap;

/**
 * @short This class implements
 * 
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class FrameImage: public KGameCanvasWidget
{
	Q_OBJECT
public:
    /**
     * Constructor
       
       @param parent 
       @param initialImageSize
    */
	FrameImage(QWidget *parent, const QSize& initialImageSize);
    /**
     * Default Destructor */
	~FrameImage();
    /**
     * Method Description
     * 
     * @return QPixmap
     */
	QPixmap *getPreviewPixmap() {return thePixmap;}
    /**
     * Method Description 

       @param x
       @param y
       @param w
       @param h
       @param ss
       @param type
     */
	void setRect(int x, int y, int w, int h, int ss, int type);
signals:
    /**
     * Signal Description */
	void mousePressed(QMouseEvent *e);
    /**
     * Signal Description */
	void mouseMoved(QMouseEvent *e);
protected:
    /**
     * Event Description */
	void mousePressEvent(QMouseEvent *e);
    /**
     * Event Description */
	void mouseMoveEvent(QMouseEvent *e);
    /**
     * Event Description */
	void resizeEvent(QResizeEvent *e);
    /**
     * Event Description */
	void paintEvent( QPaintEvent* pa );
private:
	QPixmap *thePixmap;
	int rx;
	int ry;
	int rw;
	int rh;
	int rs;
	int rt;
};


/**
* This class implements
* 
* longer description
*
* @author Mauricio Piacentini  <mauricio@tabuleiro.com>
*/
class Preview: public KDialog
{
    Q_OBJECT

public:
    /**
     * @short Enum Description */
	enum PreviewType {
        background, /**< Member Description */
        tileset,    /**< Member Description */
        board,      /**< Member Description */
        theme       /**< Member Description */
    };

    /**
     * Constructor */
	Preview(QWidget* parent);
    /**
     * Default Destructor */
	~Preview();

    /**
     * Method Description @param type  */
	void initialise(const PreviewType type);
    /**
     * Method Description */
	void saveTheme();

protected:
    /**
     * Method Description */
	void markUnchanged();
    /**
     * Method Description */
	void markChanged();
    /**
     * Method Description 
       @return @c true  if
       @return @c false if
     */
	bool isChanged();
    /**
     * Method Description @return preview QPixmap */
	QPixmap *getPreviewPixmap() {return m_drawFrame->getPreviewPixmap(); }
    /**
     * Method Description */
	virtual void drawPreview();
    /**
     * Method Description */
	void applyChange() ;
    /**
     * Method Description @param bg */
	void renderBackground(const QString &bg);
    /**
     * Method Description 
       @param file
       @param layout
     */
	void renderTiles(const QString &file, const QString &layout);
    /**
     * Event Description */
	void paintEvent( QPaintEvent* pa );

signals:
    /**
     * Signal Description */
	void boardRedraw(bool);
    /**
     * Signal Description */
	void loadTileset(const QString &);
    /**
     * Signal Description */
	void loadBackground(const QString &, bool);
    /**
     * Signal Description */
	void loadBoard(const QString &);
    /**
     * Signal Description */
	void layoutChange();

public slots:
    /**
     * Slot Description @param which */
	void selectionChanged(int which);

protected slots:
    /**
     * Slot Description @param button */
	virtual void slotButtonClicked(int button); // reimp
	
private slots:
	void load();

protected:
	FrameImage *m_drawFrame;  /**< Member Description */
	QComboBox *m_combo; /**< Member Description */

	QString m_selectedFile;     /**< Member Description */
	KMahjonggTileset m_tiles;   /**< Member Description */
	BoardLayout m_boardLayout;  /**< Member Description */
	KMahjonggBackground m_back;  /**< Member Description */

private:
	QString m_fileSelector;
	bool m_changed;
	QStringList m_fileList;
	PreviewType m_previewType;

	QString m_themeBack;
	QString m_themeLayout;
	QString m_themeTileset;
};

#endif
