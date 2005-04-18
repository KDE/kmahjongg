#include <sys/param.h>

#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kstdguiitem.h>

#include <qcombobox.h>
#include <qhgroupbox.h>
#include <qimage.h>
#include <qregexp.h>
#include <qpainter.h>
#include <qvbox.h>

#include "prefs.h"
#include "Preview.h"

static const char * themeMagicV1_0= "kmahjongg-theme-v1.0";

Preview::Preview(QWidget* parent) : KDialogBase(parent), tiles(true)
{
	KPushButton *loadButton;
	QGroupBox *group;
	QVBox *page;
	 
	page = new QVBox(this);

	group = new QHGroupBox(page);
	
	combo = new QComboBox(false, group);
	connect(combo, SIGNAL(activated(int)), SLOT(selectionChanged(int)));

	loadButton = new KPushButton(i18n("Load"), group);
	connect( loadButton, SIGNAL(clicked()), SLOT(load()) );
	
	drawFrame = new FrameImage(page);
	drawFrame->setFixedSize(310, 236);

	changed = false;
	
	setMainWidget(page);
	setFixedSize(sizeHint());
}

Preview::~Preview()
{
}

void Preview::selectionChanged(int which)
{
	selectedFile = fileList[which];
	drawPreview();
	drawFrame->repaint(0,0,-1,-1,false);
	markChanged();
}

bool Preview::isChanged()
{
	return changed;
}

void Preview::markChanged()
{
	changed = true;
}

void Preview::markUnchanged()
{
	changed = false;
}

void Preview::initialise(const PreviewType type, const char *extension)
{
	QString tile = Prefs::tileSet();
	QString back = Prefs::background();
	QString layout = Prefs::layout();

	// set up the concept of the current file. Initialised to the preferences
	// value initially. Set the caption to indicate what we are doing
	switch (type)
	{
		case background:
			setCaption(i18n("Change Background Image"));
			selectedFile = back;
			fileSelector = i18n("*.bgnd|Background Image\n"
				     "*.bmp|Windows Bitmap File (*.bmp)\n");
		break;
		
		case tileset:
			setCaption(i18n("Change Tile Set"));
			fileSelector = i18n("*.tileset|Tile Set File\n");
			selectedFile = tile;
		break;
		
		case board:
			fileSelector = i18n("*.layout|Board Layout File\n");
			setCaption(i18n("Change Board Layout"));
			selectedFile = layout;
		break;
		
		case theme:
			fileSelector = i18n("*.theme|KMahjongg Theme\n");
			setCaption(i18n("Choose Theme"));
			selectedFile="";
			
			themeLayout="";
			themeBack="";
			themeTileset="";
		
		default:
		break;
	}
	
	fileSelector += i18n("*|All Files\n");
	enableButtonApply(type != board);

	previewType = type;
	// we start with no change indicated
	markUnchanged();

	QString kmDir;
	QDir files;
	QFileInfo *current=new QFileInfo(selectedFile);

	// we pick up system files from the kde dir
	kmDir = locate("appdata", "pics/default.tileset");

	QFileInfo f(kmDir);
	kmDir = f.dirPath();

	files.cd(kmDir);
	files.setNameFilter(extension);
	files.setFilter(QDir::Files | QDir::Readable);

	// get rid of files from the last invocation
	fileList.clear();
	combo->clear();

	// deep copy the file list as we need to keen to keep it
	QFileInfoList *list = (QFileInfoList *) files.entryInfoList();
	// put the curent entry in the returned list to test for
	// duplicates on insertion

	if (!current->fileName().isEmpty())
		list->insert(0, current);

	QFileInfo *info=list->first();
	for (unsigned int p=0; p<list->count(); p++)
	{
		bool duplicate = false;

		for (unsigned int c=0; c<fileList.count(); c++)
		{
			if (info->filePath() == fileList[c]) {
				duplicate = true;
			}
		}

		if (!duplicate)
		{
			fileList.append(info->filePath());
			combo->insertItem(info->baseName());
		}
		info=list->next();
	}

	combo->setEnabled(fileList.count());
	drawPreview();
}

void Preview::slotApply() {
	if (isChanged()) {
		applyChange();
		markUnchanged();
	}
}

void Preview::slotOk() {
	slotApply();
	accept();
}

void Preview::load() {
    KURL url = KFileDialog::getOpenURL(QString::null, fileSelector, this, i18n("Open Board Layout" ));
    if ( !url.isEmpty() ) {
        selectedFile = url.path();
        drawPreview();
        drawFrame->repaint(0,0,-1,-1,false);
        markChanged();
    }
}

// Top level preview drawing method. Background, tileset and layout
// are initialised from the preferences. Depending on the type
// of preview dialog we pick up the selected file for one of these
// chaps.

void Preview::drawPreview()
{
	QString tile = Prefs::tileSet();
	QString back = Prefs::background();
	QString layout = Prefs::layout();
	
	switch (previewType)
	{
		case background:
			back = selectedFile;
		break;
		
		case tileset:
			tile = selectedFile;
		break;
		
		case board:
			layout = selectedFile;
		break;
		
		case theme:
			// a theme is quite a bit of work. We load the
			// specified bits in (layout, background and tileset
			if (!selectedFile.isEmpty())
			{
				char backRaw[MAXPATHLEN];
				char layoutRaw[MAXPATHLEN];
				char tilesetRaw[MAXPATHLEN];
				char magic[MAXPATHLEN];
				
				QFile in(selectedFile);
				if (in.open(IO_ReadOnly))
				{
					in.readLine(magic, MAXPATHLEN);
					if (magic[strlen(magic)-1]=='\n') magic[strlen(magic)-1]='\0';
					if (strncmp(themeMagicV1_0, magic, strlen(magic)) != 0)
					{
						in.close();
						KMessageBox::sorry(this, i18n("That is not a valid theme file."));
						break;
					}
					in.readLine(tilesetRaw, MAXPATHLEN);
					in.readLine(backRaw, MAXPATHLEN);
					in.readLine(layoutRaw, MAXPATHLEN);
					
					tile = QString("pics") + tilesetRaw;
					back = QString("pics") + backRaw;
					layout = QString("pics") + layoutRaw;
					
					layout.replace(QRegExp(":"), "/");
					layout.replace(QRegExp("\n"), QString::null);
					tile.replace(QRegExp(":"), "/");
					tile.replace(QRegExp("\n"), QString::null);
					back.replace(QRegExp(":"), "/");
					back.replace(QRegExp("\n"), QString::null);
					
					tile = locate("appdata", tile);
					back = locate("appdata", back);
					layout = locate("appdata", layout);
					
					in.close();
					
					themeBack=back;
					themeLayout=layout;
					themeTileset=tile;
				}
			}
		break;
	}
	
	renderBackground(back);
	renderTiles(tile, layout);
}

void Preview::paintEvent( QPaintEvent*  ){
  drawFrame->repaint(false);
}

// the user selected ok, or apply. This method passes the changes
// across to the game widget and if necessary forces a board redraw
// (unnecessary on layout changes since it only effects the next game)
void Preview::applyChange()
{
	switch (previewType)
	{
		case background:
			loadBackground(selectedFile, false);
		break;
		
		case tileset:
			loadTileset(selectedFile);
		break;
		
		case board:
			loadBoard(selectedFile);
		break;
		
		case theme:
			if (!themeLayout.isEmpty() && !themeBack.isEmpty() && !themeTileset.isEmpty())
			{
				loadBackground(themeBack, false);
				loadTileset(themeTileset);
				loadBoard(themeLayout);
			}
		break;
    }

	// don't redraw for a layout change
	if (previewType == board  || previewType == theme) layoutChange();
	else boardRedraw(true);

	// either way we mark the current value as unchanged
	markUnchanged();
}

// Render the background to the pixmap.
void Preview::renderBackground(const QString &bg) {
   QImage img;
   QImage tmp;
   QPixmap *p;
   QPixmap *b;
   p = drawFrame->getPreviewPixmap();
   back.load(bg, p->width(), p->height());
   b = back.getBackground();
   bitBlt( p, 0,0,
            b,0,0, b->width(), b->height(), CopyROP );
}

// This method draws a mini-tiled board with no tiles missing.

void Preview::renderTiles(const QString &file, const QString &layout) {
    tiles.loadTileset(file, true);
    boardLayout.loadBoardLayout(layout);

    QPixmap *dest = drawFrame->getPreviewPixmap();
    int xOffset = tiles.width()/2;
    int yOffset = tiles.height()/2;
    short tile = 0;

    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z=0; z<BoardLayout::depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < BoardLayout::height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x=BoardLayout::width-1; x>=0; x--) {
                int sx = x*(tiles.qWidth()  )+xOffset;
                int sy = y*(tiles.qHeight()  )+yOffset;
                if (boardLayout.getBoardData(z, y, x) != '1') {
                    continue;
                }
                QPixmap *t = tiles.unselectedPixmaps(tile);

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border

                if ((x>1) && (y>0) && boardLayout.getBoardData(z,y-1,x-2)=='1'){
                    bitBlt( dest, sx+2, sy,
                        t, 2,0, t->width(), t->height()/2, CopyROP );
                    bitBlt( dest, sx, sy+t->height()/2,
			t, 0,t->height()/2,t->width(),t->height()/2,CopyROP);
                } else {

                bitBlt( dest, sx, sy,
                    t, 0,0, t->width(), t->height(), CopyROP );
                }
                tile++;
                if (tile == 35)
                    tile++;
                tile = tile % 43;
            }
        }
        xOffset +=tiles.shadowSize();
        yOffset -=tiles.shadowSize();
    }
}

// this really does not belong here. It will be fixed in v1.1 onwards
void Preview::saveTheme() {
    QString tile = Prefs::tileSet();
    QString back = Prefs::background();
    QString layout = Prefs::layout();
    
    QString with = ":";
    // we want to replace any path in the default store
    // with a +
    QRegExp p(locate("data_dir", "/kmahjongg/pics/"));

    back.replace(p,with);
    tile.replace(p,with);
    layout.replace(p,with);


    // Get the name of the file to save
    KURL url = KFileDialog::getSaveURL(
        NULL,
        "*.theme",
        parentWidget(),
        i18n("Save Theme" ));
    if ( url.isEmpty() )
        return;

   if( !url.isLocalFile() )
   {
      KMessageBox::sorry( this, i18n( "Only saving to local files currently supported." ) );
      return;
   }

    // Are we over writing an existin file, or was a directory selected?
    QFileInfo f( url.path() );
    if( f.isDir() )
        return;
    if (f.exists()) {
        // if it already exists, querie the user for replacement
        int res=KMessageBox::warningYesNo(this,
                        i18n("A file with that name "
                                           "already exists. Do you "
                                           "wish to overwrite it?"));
        if (res != KMessageBox::Yes)
                return ;
    }
    FILE *outFile = fopen( QFile::encodeName(url.path()), "w" );
    if (outFile == NULL) {
        KMessageBox::sorry(this,
                i18n("Could not write to file. Aborting."));
        return;
    }

    fprintf(outFile,"%s\n%s\n%s\n%s\n",
		themeMagicV1_0,
		tile.utf8().data(),
		back.utf8().data(),
		layout.utf8().data());
    fclose(outFile);
}

FrameImage::FrameImage (QWidget *parent, const char *name)
  : QFrame(parent, name)
{
	rx = -1;
	thePixmap = new QPixmap();
}

FrameImage::~FrameImage()
{
	delete thePixmap;
}

void FrameImage::setGeometry(int x, int y, int w, int h) {
    QFrame::setGeometry(x,y,w,h);

    thePixmap->resize(size());

}

void FrameImage::paintEvent( QPaintEvent* pa )
{
    QFrame::paintEvent(pa);

    QPainter p(this);


    QPen line;
    line.setStyle(DotLine);
    line.setWidth(2);
    line.setColor(yellow);
    p.setPen(line);
    p.setBackgroundMode(OpaqueMode);
    p.setBackgroundColor(black);

    int x = pa->rect().left();
    int y = pa->rect().top();
    int h = pa->rect().height();
    int w  = pa->rect().width();

    p.drawPixmap(x+frameWidth(),y+frameWidth(),*thePixmap,x+frameWidth(),y+frameWidth(),w-(2*frameWidth()),h-(2*frameWidth()));
    if (rx >=0) {

	p.drawRect(rx, ry, rw, rh);
	p.drawRect(rx+rs, ry, rw-rs, rh-rs);
	p.drawLine(rx, ry+rh, rx+rs, ry+rh-rs);

	int midX = rx+rs+((rw-rs)/2);
	int midY = ry+((rh-rs)/2);
	switch (rt) {
		case 0:  // delete mode cursor
			p.drawLine(rx+rs, ry, rx+rw, ry+rh-rs);
			p.drawLine(rx+rw, ry, rx+rs, ry+rh-rs);
		break;
		case 1: // insert cursor
			p.drawLine(midX, ry, midX, ry+rh-rs);
			p.drawLine(rx+rs, midY, rx+rw, midY);
		break;
		case 2: // move mode cursor
			p.drawLine(midX, ry, rx+rw, midY);
			p.drawLine(rx+rw, midY, midX, ry+rh-rs);
			p.drawLine(midX, ry+rh-rs, rx+rs, midY);
			p.drawLine(rx+rs, midY, midX, ry);

		break;
	}

    }
}

void FrameImage::setRect(int x,int y,int w,int h, int s, int t)
{
	rx = x;
	ry = y;
	rw = w;
	rh = h;
	rt = t;
	rs = s;
}

// Pass on the mouse presed event to our owner

void FrameImage::mousePressEvent(QMouseEvent *m) {
	mousePressed(m);
}

void FrameImage::mouseMoveEvent(QMouseEvent *e) {
	mouseMoved(e);
}

#include "Preview.moc"
