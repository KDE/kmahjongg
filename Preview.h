#ifndef _PreviewLoadBase_H
#define _PreviewLoadBase_H

#include <kdialogbase.h>

#include <qframe.h>

#include "Tileset.h"
#include "BoardLayout.h"
#include "Background.h"

class QComboBox;
class QPixmap;

class FrameImage: public QFrame
{
	Q_OBJECT
public:
	FrameImage(QWidget *parent=NULL, const char *name = NULL);
	~FrameImage();
	void setGeometry(int x, int y, int w, int h);
	QPixmap *getPreviewPixmap() {return thePixmap;}
	void setRect(int x, int y, int w, int h, int ss, int type);
signals:
	void mousePressed(QMouseEvent *e);
	void mouseMoved(QMouseEvent *e);
protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
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



class Preview: public KDialogBase
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
	void slotApply();
	void slotOk();
	
private slots:
	void load();

protected:
	FrameImage *m_drawFrame;
	QComboBox *m_combo;

	QString m_selectedFile;
	Tileset m_tiles;
	BoardLayout m_boardLayout;
	Background m_back;

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
