
#include <qlabel.h>
#include <qpixmap.h>
#include "Progress.moc"
#include <kstandarddirs.h>                       



Progress::Progress
(
	QWidget* parent,
	const char* name
)
	:
	QDialog( parent, name, false, WStyle_Customize | WStyle_NoBorder )
{
	pix = new QPixmap();
	// get the picture
	QString kmDir = locate("appdata", "pics/splash.png");
	if (!kmDir.isNull()) {
		pix->load(kmDir);;

	} else {
		pix->resize(200,100);
	}

	setBackgroundPixmap(*pix);
	text = new QLabel("kslkslksl", this);
	text->setGeometry(10,80,200,15);

	resize( 200, 100);
	setFixedSize(200, 100);
}

void Progress::status(const QString &s) {
	text->setText(s);

}

	
Progress::~Progress()
{
}







