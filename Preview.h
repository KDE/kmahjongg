#ifndef _PreviewLoadBase_H 
#define _PreviewLoadBase_H 

#include <qdialog.h>
#include <qframe.h>   

#include <qfileinfo.h>
#include "Tileset.h"
#include "BoardLayout.h"
#include "Background.h"

class QPushButton;
class QComboBox;
class QButtonGroup;
class QPixmap;

class FrameImage: public QFrame
{
	Q_OBJECT
public:
	FrameImage(QWidget *parent=NULL, const char *name = NULL);
	void setGeometry(int x, int y, int w, int h);
	QPixmap *getPreviewPixmap(void) {return thePixmap;};
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



class Preview: public QDialog
{
    Q_OBJECT

public:

    enum PreviewType {background, tileset, board, theme};

    Preview
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~Preview();

    void initialise(const PreviewType type, const char *extension);
	void saveTheme(void);


protected:
	void markUnchanged(void);
	void markChanged(void);
	bool isChanged(void);
	QPixmap *getPreviewPixmap(void) {return drawFrame->getPreviewPixmap();};	
	virtual void drawPreview(void);
        void applyChange(void) ;
        void renderBackground(const QString &bg);
        void renderTiles(const QString &file, const QString &layout); 
	void paintEvent( QPaintEvent* pa );
signals:
        void boardRedraw(bool);
        void loadTileset(const QString &);
        void loadBackground(const QString &, bool);
	void loadBoard(const QString &);
	void layoutChange(void);
 

public slots:
	void selectionChanged(int which);

protected slots:
	
private slots:
	void apply(void);
	void ok(void);
	void load(void);
	
protected:
        FrameImage * drawFrame;
        QPushButton* cancelButton;
        QPushButton* applyButton;
        QPushButton* okButton;
        QPushButton* loadButton;
        QComboBox* combo;
        QButtonGroup* topGroup;
        QButtonGroup* bottomGroup;

	QString selectedFile;
        Tileset tiles;
        BoardLayout boardLayout; 
	Background back;
 
private:
	void parseFile(const QString &f, QString &g);
	QString fileSelector;
	bool changed;
	QFileInfoList	fileList;
	PreviewType previewType;
	
	QString themeBack;
	QString themeLayout;
	QString themeTileset;
};

#endif 
