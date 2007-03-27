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

class FrameImage: public KGameCanvasWidget
{
	Q_OBJECT
public:
	FrameImage(QWidget *parent, const QSize& initialImageSize);
	~FrameImage();
	QPixmap *getPreviewPixmap() {return thePixmap;}
	void setRect(int x, int y, int w, int h, int ss, int type);
signals:
	void mousePressed(QMouseEvent *e);
	void mouseMoved(QMouseEvent *e);
protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void resizeEvent(QResizeEvent *e);
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



class Preview: public KDialog
{
    Q_OBJECT

public:
	enum PreviewType {background, tileset, board, theme};

	Preview(QWidget* parent);
	~Preview();

	void initialise(const PreviewType type);

	void saveTheme();

protected:
	void markUnchanged();
	void markChanged();
	bool isChanged();
	QPixmap *getPreviewPixmap() {return m_drawFrame->getPreviewPixmap(); }
	virtual void drawPreview();
	void applyChange() ;
	void renderBackground(const QString &bg);
	void renderTiles(const QString &file, const QString &layout);
	void paintEvent( QPaintEvent* pa );

signals:
	void boardRedraw(bool);
	void loadTileset(const QString &);
	void loadBackground(const QString &, bool);
	void loadBoard(const QString &);
	void layoutChange();

public slots:
	void selectionChanged(int which);

protected slots:
	virtual void slotButtonClicked(int button); // reimp
	
private slots:
	void load();

protected:
	FrameImage *m_drawFrame;
	QComboBox *m_combo;

	QString m_selectedFile;
	KMahjonggTileset m_tiles;
	BoardLayout m_boardLayout;
	KMahjonggBackground m_back;

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
