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

#include <kapplication.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kstdguiitem.h>
#include <kimageio.h>

#include <QComboBox>
#include <QGroupBox>
#include <qevent.h>
#include <qimage.h>
#include <QRegExp>
#include <qpainter.h>
#include <qtextstream.h>
#include <kvbox.h>


#include "prefs.h"
#include "Preview.h"
#include <QVBoxLayout>
static const char * themeMagicV1_0= "kmahjongg-theme-v1.0";

Preview::Preview(QWidget* parent) : KDialog(parent), m_tiles()
{
	KPushButton *loadButton;
	QGroupBox *group;

	group = new QGroupBox( this );

	m_combo = new QComboBox(group);
	connect(m_combo, SIGNAL(activated(int)), SLOT(selectionChanged(int)));

	loadButton = new KPushButton(i18n("Load..."), group);
	connect( loadButton, SIGNAL(clicked()), SLOT(load()) );

	QSize frameSize(310, 236);
	m_drawFrame = new FrameImage(group, frameSize);
	m_drawFrame->setFixedSize(frameSize);
	m_changed = false;

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget( m_combo );
        layout->addWidget(loadButton );
        layout->addWidget(m_drawFrame );

        group->setLayout( layout );
	setMainWidget(group);
	setFixedSize(sizeHint());
}

Preview::~Preview()
{
}

void Preview::selectionChanged(int which)
{
	m_selectedFile = m_fileList[which];
	drawPreview();
	m_drawFrame->update();
	markChanged();
}

bool Preview::isChanged()
{
	return m_changed;
}

void Preview::markChanged()
{
	m_changed = true;
}

void Preview::markUnchanged()
{
	m_changed = false;
}

void Preview::initialise(const PreviewType type)
{
	QString extension;
	QString tile = Prefs::tileSet();
	QString back = Prefs::background();
	QString layout = Prefs::layout();

	// set up the concept of the current file. Initialised to the preferences
	// value initially. Set the caption to indicate what we are doing
	switch (type)
	{
		case background:
			setCaption(i18n("Change Background Image"));
			m_selectedFile = back;
			m_fileSelector = i18n("*.bgnd|Background Image (*.bgnd)\n");
			m_fileSelector += KImageIO::pattern()+'\n';
			extension = "*.bgnd";
		break;

		case tileset:
			setCaption(i18n("Change Tile Set"));
			m_fileSelector = i18n("*.tileset|Tile Set File (*.tileset)\n");
			m_selectedFile = tile;
			extension = "*.tileset";
		break;

		case board:
			m_fileSelector = i18n("*.layout|Board Layout File (*.layout)\n");
			setCaption(i18n("Change Board Layout"));
			m_selectedFile = layout;
			extension = "*.layout";
		break;

		case theme:
			m_fileSelector = i18n("*.theme|KMahjongg Theme File (*.theme)\n");
			setCaption(i18n("Choose Theme"));
			m_selectedFile="";
			extension = "*.theme";

			m_themeLayout="";
			m_themeBack="";
			m_themeTileset="";

		default:
		break;
	}

	m_fileSelector += i18n("*|All Files");
	enableButtonApply(type != board);

	m_previewType = type;
	// we start with no change indicated
	markUnchanged();

	m_fileList = kapp->dirs()->findAllResources("appdata",  "pics/*"+extension, false, true);

	// get rid of files from the last invocation
	m_combo->clear();

	QStringList names;
	QStringList::const_iterator it, itEnd;
	it = m_fileList.begin();
	itEnd = m_fileList.end();
	for ( ; it != itEnd; ++it)
	{
		QFileInfo fi(*it);
		names << fi.baseName();
	}

	m_combo->addItems(names);
	m_combo->setEnabled(m_fileList.count());
	drawPreview();
}

void Preview::slotButtonClicked(int button)
{
	if(button == KDialog::Cancel)
	{
		reject();
		return;
	}
	//common part for Ok & Apply
	if (isChanged()) {
		applyChange();
		markUnchanged();
	}

	if(button == KDialog::Ok)
		accept();
}

void Preview::load() {
    KUrl url = KFileDialog::getOpenUrl(KUrl(), m_fileSelector, this, i18n("Open Board Layout" ));
    if ( !url.isEmpty() ) {
        m_selectedFile = url.path();
        drawPreview();
        m_drawFrame->update();
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

	switch (m_previewType)
	{
		case background:
			back = m_selectedFile;
		break;

		case tileset:
			tile = m_selectedFile;
		break;

		case board:
			layout = m_selectedFile;
		break;

		case theme:
			// a theme is quite a bit of work. We load the
			// specified bits in (layout, background and tileset
			if (!m_selectedFile.isEmpty())
			{
				QString backRaw, layoutRaw, tilesetRaw, magic;

				QFile in(m_selectedFile);
				if (in.open(QIODevice::ReadOnly))
				{
					QTextStream stream(&in);
					magic = stream.readLine();
					if (magic != themeMagicV1_0)
					{
						in.close();
						KMessageBox::sorry(this, i18n("That is not a valid theme file."));
						break;
					}
					tilesetRaw = stream.readLine();
					backRaw = stream.readLine();
					layoutRaw = stream.readLine();
					in.close();

					tile = tilesetRaw;
					tile.replace(":", "/kmahjongg/pics/");
					if (!QFile::exists(tile))
					{
						tile = tilesetRaw;
						tile = "pics/" + tile.right(tile.length() - tile.indexOf(":") - 1 );
						tile = KStandardDirs::locate("appdata", tile);
					}

					back = backRaw;
					back.replace(":", "/kmahjongg/pics/");
					if (!QFile::exists(back))
					{
						back = backRaw;
						back = "pics/" + back.right(back.length() - back.indexOf(":") - 1);
						back = KStandardDirs::locate("appdata", back);
					}

					layout = layoutRaw;
					layout.replace(":", "/kmahjongg/pics/");
					if (!QFile::exists(layout))
					{
						layout = layoutRaw;
						layout = "pics/" + layout.right(layout.length() - layout.indexOf(":") - 1);
						layout = KStandardDirs::locate("appdata", layout);
					}

					m_themeBack=back;
					m_themeLayout=layout;
					m_themeTileset=tile;
				}
			}
		break;
	}

	renderBackground(back);
	renderTiles(tile, layout);
}

void Preview::paintEvent( QPaintEvent*  ){
  m_drawFrame->update();
}

// the user selected ok, or apply. This method passes the changes
// across to the game widget and if necessary forces a board redraw
// (unnecessary on layout changes since it only effects the next game)
void Preview::applyChange()
{
	switch (m_previewType)
	{
		case background:
			loadBackground(m_selectedFile, false);
		break;

		case tileset:
			loadTileset(m_selectedFile);
		break;

		case board:
			loadBoard(m_selectedFile);
		break;

		case theme:
			if (!m_themeLayout.isEmpty() && !m_themeBack.isEmpty() && !m_themeTileset.isEmpty())
			{
				loadBackground(m_themeBack, false);
				loadTileset(m_themeTileset);
				loadBoard(m_themeLayout);
			}
		break;
    }

	// don't redraw for a layout change
	if (m_previewType == board  || m_previewType == theme) layoutChange();
	else boardRedraw(true);

	// either way we mark the current value as unchanged
	markUnchanged();
}

// Render the background to the pixmap.
void Preview::renderBackground(const QString &bg) {
   QImage img;
   QImage tmp;
   QPixmap *p;
   QPixmap b;
   p = m_drawFrame->getPreviewPixmap();
   m_back.load(bg, p->width(), p->height());
   //TODO: this class needs to be revamped, use Brush texture for now
   b = m_back.getBackground().texture();
   QPainter paint(p);
   paint.drawPixmap(0,0, b);
}

// This method draws a mini-tiled board with no tiles missing.

void Preview::renderTiles(const QString &file, const QString &layout) {
    m_tiles.loadTileset(file);
    m_boardLayout.loadBoardLayout(layout);

    QPixmap *dest = m_drawFrame->getPreviewPixmap();
    QSize newtilesize = m_tiles.preferredTileSize(dest->size(), m_boardLayout.m_width/2, m_boardLayout.m_height/2);
    m_tiles.reloadTileset(newtilesize);

    int xOffset = (dest->width() - (m_boardLayout.m_width*(m_tiles.qWidth())) - (m_tiles.width()-(m_tiles.qWidth()*2)))/2;
    int yOffset = (dest->height() - (m_boardLayout.m_height*(m_tiles.qHeight())) - (m_tiles.height()-(m_tiles.qHeight()*2)))/2;
    short tile = 0;

    QPainter p(dest);
    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z=0; z< m_boardLayout.m_depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < m_boardLayout.m_height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x= m_boardLayout.m_width-1; x>=0; x--) {
                int sx = x*(m_tiles.qWidth()  )+xOffset;
                int sy = y*(m_tiles.qHeight()  )+yOffset;
                if (m_boardLayout.getBoardData(z, y, x) != '1') {
                    continue;
                }
                QPixmap t = m_tiles.unselectedTile(0); //was(tile)

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border

                if ((x>1) && (y>0) && m_boardLayout.getBoardData(z,y-1,x-2)=='1'){
                    p.drawPixmap( sx+2, sy,
                        t, 2,0, t.width(), t.height()/2 );
                    p.drawPixmap( sx, sy+t.height()/2,
			t, 0,t.height()/2,t.width(),t.height()/2);
                } else {

                p.drawPixmap( sx, sy,
                    t, 0,0, t.width(), t.height() );
                }
                tile++;
                if (tile == 35)
                    tile++;
                tile = tile % 43;
            }
        }
        xOffset +=m_tiles.levelOffset();
        yOffset -=m_tiles.levelOffset();
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
    QRegExp p(KStandardDirs::locate("data_dir", "/kmahjongg/pics/"));

    back.replace(p,with);
    tile.replace(p,with);
    layout.replace(p,with);


    // Get the name of the file to save
    KUrl url = KFileDialog::getSaveUrl(
        KUrl(),
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
        int res=KMessageBox::warningContinueCancel(this,
                        i18n("A file with that name "
                                           "already exists. Do you "
                                           "wish to overwrite it?"),QString::null,KGuiItem(i18n("Overwrite")));
        if (res != KMessageBox::Continue)
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
		tile.toUtf8().constData(),
		back.toUtf8().constData(),
		layout.toUtf8().constData());
    fclose(outFile);
}

FrameImage::FrameImage (QWidget *parent, const QSize& initialImageSize)
  : KGameCanvasWidget(parent)
{
	rx = -1;
	thePixmap = new QPixmap(initialImageSize);
}

FrameImage::~FrameImage()
{
	delete thePixmap;
}

void FrameImage::resizeEvent( QResizeEvent* ev )
{
    *thePixmap = QPixmap(ev->size());
}

void FrameImage::paintEvent( QPaintEvent* pa )
{
    //QFrame::paintEvent(pa);

    QPainter p(this);


    QPen line;
    line.setStyle(Qt::DotLine);
    line.setWidth(2);
    line.setColor(Qt::yellow);
    p.setPen(line);
    p.setBackgroundMode(Qt::OpaqueMode);
    p.setBackground(Qt::black);

    int x = pa->rect().left();
    int y = pa->rect().top();
    int h = pa->rect().height();
    int w  = pa->rect().width();

    //p.drawPixmap(x+frameWidth(),y+frameWidth(),*thePixmap,x+frameWidth(),y+frameWidth(),w-(2*frameWidth()),h-(2*frameWidth()));
p.drawPixmap(x,y,*thePixmap,x,y,w-(2),h-(2));
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
	emit mousePressed(m);
}

void FrameImage::mouseMoveEvent(QMouseEvent *e) {
	emit mouseMoved(e);
}

#include "Preview.moc"
