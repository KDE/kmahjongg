#include <sys/param.h>
#include <kapplication.h>
#include <qimage.h>
#include <qregexp.h>
#include <kfiledialog.h>
#include <qpainter.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include "prefs.h"
#include "Preview.h"

static const char * themeMagicV1_0= "kmahjongg-theme-v1.0";

Preview::Preview
(
	QWidget* parent,
	const char* name
)
	:
	QDialog( parent, name, TRUE, 0 ), tiles(true)
{
	const int dx=0;
	const int dy = 66;

	bottomGroup = new QButtonGroup( this, "bottomGroup" );
	bottomGroup->setGeometry( 10, 230+dy, 310+dx, 50 );
	bottomGroup->setMinimumSize( 0, 0 );
	bottomGroup->setMaximumSize( 32767, 32767 );
	bottomGroup->setFocusPolicy( QWidget::NoFocus );
	bottomGroup->setBackgroundMode( QWidget::PaletteBackground );
	bottomGroup->setFrameStyle( 49 );
	bottomGroup->setTitle( "" );
	bottomGroup->setAlignment( 1 );

	topGroup = new QButtonGroup( this, "topGroup" );
	topGroup->setGeometry( 10, 10, 310+dx, 50);
	topGroup->setMinimumSize( 0, 0 );
	topGroup->setMaximumSize( 32767, 32767 );
	topGroup->setFocusPolicy( QWidget::NoFocus );
	topGroup->setBackgroundMode( QWidget::PaletteBackground );
	topGroup->setFrameStyle( 49 );
	topGroup->setTitle( "" );
	topGroup->setAlignment( 1 );

	combo = new QComboBox( FALSE, this, "combo" );
	combo->setGeometry( 20, 20, 220+dx, 25 );
	combo->setMinimumSize( 0, 0 );
	combo->setMaximumSize( 32767, 32767 );
	combo->setFocusPolicy( QWidget::StrongFocus );
	combo->setBackgroundMode( QWidget::PaletteBackground );
	combo->setSizeLimit( 10 );
	connect( combo, SIGNAL(activated(int)), SLOT(selectionChanged(int)) );

	loadButton = new QPushButton( this, "loadButton" );
	loadButton->setGeometry( 250+dx, 20, 61, 26 );
	loadButton->setMinimumSize( 0, 0 );
	loadButton->setMaximumSize( 32767, 32767 );
	connect( loadButton, SIGNAL(clicked()), SLOT(load()) );
	loadButton->setFocusPolicy( QWidget::TabFocus );
	loadButton->setBackgroundMode( QWidget::PaletteBackground );
	loadButton->setText(i18n("Load"));
	loadButton->setAutoRepeat( FALSE );

	//       total w  - button tot wid /4 (space left)
	int bw = ((310+dx) - ((90+(dx/3))*3))/4 ;
	int pos = bw +10;

	okButton = new KPushButton( KStdGuiItem::ok(), this, "okButton" );
	okButton->setGeometry( pos, 240+dy, 90+(dx/3), 26 ); // was 20
	okButton->setMinimumSize( 0, 0 );
	okButton->setMaximumSize( 32767, 32767 );
	connect( okButton, SIGNAL(clicked()), SLOT(ok()) );
	okButton->setFocusPolicy( QWidget::TabFocus );
	okButton->setBackgroundMode( QWidget::PaletteBackground );
	okButton->setAutoRepeat( FALSE );
        okButton->setDefault(true);

	pos += (90+(dx/3))+bw;
	applyButton = new KPushButton( KStdGuiItem::apply(), this, "applyButton" );
	applyButton->setGeometry( pos, 240+dy, 90+(dx/3), 26 );
	applyButton->setMinimumSize( 0, 0 );
	applyButton->setMaximumSize( 32767, 32767 );
	connect( applyButton, SIGNAL(clicked()), SLOT(apply()) );
	applyButton->setFocusPolicy( QWidget::TabFocus );
	applyButton->setBackgroundMode( QWidget::PaletteBackground );
	applyButton->setAutoRepeat( FALSE );

	pos += (90+(dx/3))+bw;
	cancelButton = new KPushButton( KStdGuiItem::cancel(), this, "cancelButton" );
	cancelButton->setGeometry( pos, 240+dy, 90+(dx/3), 26 );
	cancelButton->setMinimumSize( 0, 0 );
	cancelButton->setMaximumSize( 32767, 32767 );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
	cancelButton->setFocusPolicy( QWidget::TabFocus );
	cancelButton->setBackgroundMode( QWidget::PaletteBackground );
	cancelButton->setAutoRepeat( FALSE );

	drawFrame = new FrameImage( this, "drawFrame" );
	drawFrame->setGeometry( 10, 60, 310+dx, 170+dy );
	drawFrame->setMinimumSize( 0, 0 );
	drawFrame->setMaximumSize( 32767, 32767 );
	drawFrame->setFocusPolicy( QWidget::NoFocus );
	drawFrame->setBackgroundMode( QWidget::PaletteBackground );
	drawFrame->setFrameStyle( 49 );


	bottomGroup->insert( okButton );
	bottomGroup->insert( applyButton );
	bottomGroup->insert( cancelButton );

	topGroup->insert( loadButton );

	resize( 330+dx, 290+dy );
	setMinimumSize( 330+dx, 290+dy );
	setMaximumSize( 330+dx, 290+dy );

	changed = false;
}


Preview::~Preview()
{
}

void Preview::selectionChanged(int which) {
	QFileInfo *f= fileList.at(which);

	selectedFile = f->filePath();
	drawPreview();
	drawFrame->repaint(0,0,-1,-1,false);
	markChanged();
}

bool Preview::isChanged(void)
{
	return changed;
}

void Preview::markChanged(void)
{
	changed = true;
}

void Preview::markUnchanged(void)
{
	changed = false;
}

void Preview::initialise(const PreviewType type, const char *extension)
{
    QString tile = "pics/" + Prefs::tileSet();
    QString back = "pics/" + Prefs::background();
    QString layout = "pics/" + Prefs::layout();
    tile = locate("appdata", tile);
    back = locate("appdata", back);
    layout = locate("appdata", layout);

	// set up the concept of the current file. Initialised to the preferences
	// value initially. Set the caption to indicate what we are doing
	switch (type) {
	    case background:
		      setCaption(kapp->makeStdCaption(i18n("Change Background Image")));
		      selectedFile = back;
		      fileSelector = i18n("*.bgnd|Background Image\n"
				     "*.bmp|Windows bitmap file (*.bmp)\n");
		  break;
            case tileset:
		      setCaption(kapp->makeStdCaption(i18n("Change Tile Set")));
		      fileSelector = i18n("*.tileset|Tile set file\n");
		      selectedFile = tile;
		  break;
            case board:
		      fileSelector = i18n("*.layout|Board layout file\n");
		      setCaption(kapp->makeStdCaption(i18n("Change Board Layout")));
		      selectedFile = layout;
	          break;

	    case theme:
		     fileSelector = i18n("*.theme|KMahjongg theme\n");
		     setCaption(kapp->makeStdCaption(i18n("Choose Theme")));
		     selectedFile="";

		     themeLayout="";
		     themeBack="";
		     themeTileset="";

            default:
                  break;
	}
	fileSelector += i18n("*|All Files\n");
	applyButton->setEnabled(type != board);


	previewType = type;
	// we start with no change indicated
	markUnchanged();

	QString kmDir;
	QDir files;
	QFileInfo *current=new QFileInfo(selectedFile);
	bool oneFound = false;

	// we pick up system files from the kde dir
	kmDir = locate("appdata", "pics/default.tileset");

	QFileInfo f(kmDir);
	kmDir = f.dirPath();


	files.cd(kmDir);
	files.setNameFilter(extension);
	files.setFilter(QDir::Files | QDir::Readable);

	// get rid of files from the last invocation
	fileList.clear();

	// deep copy the file list as we need to keen to keep it
	QFileInfoList *list = (QFileInfoList *) files.entryInfoList();
	// put the curent entry in the returned list to test for
	// duplicates on insertion


	if (!current->fileName().isEmpty())
		list->insert(0, current);

	QFileInfo *info=list->first();
	for (unsigned int p=0; p<list->count(); p++) {

		int duplicate = 0;

		for (unsigned int c=0; c<fileList.count(); c++) {
			if (info->fileName() == fileList.at(c)->fileName()) {
				duplicate = 1;
			}
		}


		if (!duplicate) {
			fileList.append( new QFileInfo(*info));
		}
		info=list->next();
	}

	// copy the file basenames into the combo box
	combo->clear();
	if (fileList.count() >0) {
		QFileInfo *cur=fileList.first();
		for (unsigned int each=0; each < fileList.count(); each++) {
			QString insert = cur->baseName();
			combo->insertItem(insert);
			cur = fileList.next();
		}
		oneFound = true;
	}
	combo->setEnabled(oneFound);
	drawPreview();
}

void Preview::apply(void) {
	if (isChanged()) {
		applyChange();
		markUnchanged();
	}
}

void Preview::ok(void) {
	apply();
	accept();
}

void Preview::load(void) {
    KURL url = KFileDialog::getOpenURL(
                                NULL,
				fileSelector,
                                this,
                                i18n("Open Board Layout" ));
    if (  !url.isEmpty() ) {
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

void Preview::drawPreview(void) {
    QString tile = "pics/" + Prefs::tileSet();
    QString back = "pics/" + Prefs::background();
    QString layout = "pics/" + Prefs::layout();
    tile = locate("appdata", tile);
    back = locate("appdata", back);
    layout = locate("appdata", layout);
    
    switch (previewType) {
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
	    if (!selectedFile.isEmpty()) {
		char backRaw[MAXPATHLEN];
		char layoutRaw[MAXPATHLEN];
		char tilesetRaw[MAXPATHLEN];
		char magic[MAXPATHLEN];

		QFile in(selectedFile);
		if (in.open(IO_ReadOnly)){
		    in.readLine(magic, MAXPATHLEN);
		    if (magic[strlen(magic)-1]=='\n')
			magic[strlen(magic)-1]='\0';



		    if (strncmp(themeMagicV1_0, magic, strlen(magic)) != 0) {
			in.close();

        		KMessageBox::sorry(this,
                		i18n("That is not a valid theme file."));
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

		    //parseFile(tilesetRaw, tile);
		    //parseFile(backRaw, back);
		    //parseFile(layoutRaw, layout);

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

void Preview::parseFile(const QString &in, QString &out) {
	//QString prefix = locate("appdata", QString("pics/") +in);
	QString prefix = locate("appdata", "pics/default.tileset");
	QFileInfo f( prefix );
	prefix = f.dirPath();

	// remove any trailing \n
	QString tmp;
	tmp.replace(QRegExp("\n"), QString::null);
	tmp.replace(QRegExp(":"), prefix + '/');

	out = tmp;
}

// the user selected ok, or apply. This method passes the changes
// across to the game widget and if necessary forces a board redraw
// (unnecessary on layout changes since it only effects the next game)
void Preview::applyChange(void) {

    switch (previewType) {
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
		if (!themeLayout.isEmpty() && !themeBack.isEmpty() && !themeTileset.isEmpty()) {
		    loadBackground(themeBack, false);
		    loadTileset(themeTileset);
		    loadBoard(themeLayout);
		}
	     break;
    }

    // don't redraw for a layout change


    if (previewType == board  || previewType == theme) {
	layoutChange();
    } else {

        boardRedraw(true);
    }

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
void Preview::saveTheme(void) {
    QString tile = "pics/" + Prefs::tileSet();
    QString back = "pics/" + Prefs::background();
    QString layout = "pics/" + Prefs::layout();
    tile = locate("appdata", tile);
    back = locate("appdata", back);
    layout = locate("appdata", layout);
    
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
