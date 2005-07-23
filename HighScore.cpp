#include <unistd.h>
#include "HighScore.h"
#include "HighScore.moc"


#include <qlabel.h>
#include <qfileinfo.h>
#include <kmessagebox.h>
#include "klocale.h"
#include <kstandarddirs.h>                       
#include <kiconloader.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <kapplication.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

static const QString highScoreMagic1_0 = "kmahjongg-scores-v1.0";
static const QString highScoreMagic1_1 = "kmahjongg-scores-v1.1";  

static const char * highScoreFilename = "/kmahjonggHiscores";

const char * defNames[numScores] = {
    "David Black",
    "Mathias Mueller",
    "Osvaldo Stark",
    "Steve Taylor",
    "Clare Brizzolara",
    "Angela Simpson",
    "Michael O'Brien",
    "Kelvin Bell",
    "Jenifferi O'Keeffe",
    "Phil Lamdin"
};

int defScores[numScores] = 
	{400, 350, 300, 250, 200, 150, 100, 50, 20, 10}; 
 
const int ages = 59+(59*60)+(2*60*60);    
int defTimes[numScores] = {ages, ages-1, ages-2, ages-3,  
				ages-4, ages-5, ages-6, ages-7, ages-8, ages-9};


HighScore::HighScore
(
	QWidget* parent,
	const char* name
)
	:
	QDialog( parent, name, true, 0 )
{

	// form the target name


	filename = locateLocal("appdata", highScoreFilename);

	QFont fnt;
	// Number
	QLabel* qtarch_Label_3;
	qtarch_Label_3 = new QLabel( this, "Label_3" );
	qtarch_Label_3->setGeometry( 10, 45, 30, 30 );
	qtarch_Label_3->setFrameStyle( 50 );
	qtarch_Label_3->setText( i18n("Pos") );
	qtarch_Label_3->setAlignment( AlignCenter );
	fnt = qtarch_Label_3->font();
	fnt.setBold(true);
	qtarch_Label_3->setFont(fnt);


	// name

	QLabel* qtarch_Label_4;
	qtarch_Label_4 = new QLabel( this, "Label_4" );
	qtarch_Label_4->setGeometry( 40, 45, 150, 30 );
	qtarch_Label_4->setFrameStyle( 50 );
	qtarch_Label_4->setText( i18n("Name") );
	qtarch_Label_4->setFont(fnt);


	// board number
	QLabel* boardTitle;
	boardTitle= new QLabel( this, "" );
	boardTitle->setGeometry( 190, 45, 80, 30 );
	boardTitle->setFrameStyle( 50 );
	boardTitle->setText( i18n("Board") );
	boardTitle->setFont(fnt);

	// score
	QLabel* qtarch_Label_5;
	qtarch_Label_5 = new QLabel( this, "Label_5" );
	qtarch_Label_5->setGeometry( 270, 45, 70, 30 );
	qtarch_Label_5->setFrameStyle( 50 );
	qtarch_Label_5->setText( i18n("Score") );
	qtarch_Label_5->setFont(fnt);

        // time
        QLabel* qtarch_Label_6;
        qtarch_Label_6 = new QLabel( this, "Label_6" );
        qtarch_Label_6->setGeometry( 340, 45, 70, 30 );
        qtarch_Label_6->setFrameStyle( 50 );
        qtarch_Label_6->setText( i18n("Time") );
        qtarch_Label_6->setFont(fnt); 



	for (int row=0; row<numScores; row++)
		addRow(row);

	QPushButton* qtarch_PushButton_1;
	qtarch_PushButton_1 = new KPushButton( KStdGuiItem::ok(), this, "PushButton_1" );
	qtarch_PushButton_1->setGeometry( 110+35, 340+50, 100, 30 );
	qtarch_PushButton_1->setMinimumSize( 0, 0 );
	qtarch_PushButton_1->setMaximumSize( 32767, 32767 );
	qtarch_PushButton_1->setFocusPolicy( QWidget::TabFocus );
	qtarch_PushButton_1->setAutoRepeat( false );
        qtarch_PushButton_1->setAutoResize( false );
        qtarch_PushButton_1->setDefault(true);

	QPushButton* resetBtn;
	resetBtn= new QPushButton( this, "resetBtn" );
	resetBtn->setGeometry( 10, 5, 25, 25);
	resetBtn->setMinimumSize( 0, 0 );
	resetBtn->setMaximumSize( 32767, 32767 );
	resetBtn->setFocusPolicy( QWidget::TabFocus );
	//resetBtn->setText(i18n( "Reset" ));
	resetBtn->setAutoRepeat( false );
	resetBtn->setAutoResize( false );

 	KIconLoader *loader = KGlobal::iconLoader();
        resetBtn->setPixmap(loader->loadIcon("editdelete", KIcon::Toolbar));
       


	/* We create the ediat area for the hi score name entry and move it */
	/* off screen. it is moved over and placed in position when a */
	/* new name is added */

	lineEdit = new QLineEdit(this, "");
	lineEdit->setGeometry( 50, 40+(20*30), 190, 30 );
	lineEdit->setFocusPolicy(QWidget::StrongFocus);
	lineEdit->setFrame(true);
	lineEdit->setEchoMode(QLineEdit::Normal);
	lineEdit->setText("");

	// the drop down for the board names

        combo = new QComboBox( false, this, "combo" );
        combo->setGeometry( 65, 5, 220, 25 );
        combo->setMinimumSize( 0, 0 );
        combo->setMaximumSize( 32767, 32767 );
        combo->setFocusPolicy( QWidget::StrongFocus );
        combo->setSizeLimit( 10 );
        combo->setAutoResize( false );
        connect( combo, SIGNAL(activated(int)), SLOT(selectionChanged(int)) );


	resize( 350+70,390+45 );
	setFixedSize(350+70,390+45);

	tables = NULL; 
	loadTables();
	currTable = tables;

	setCaption(kapp->makeStdCaption(i18n("Scores")));

	selectedLine = -1;	

	connect(lineEdit, SIGNAL( textChanged(const QString &)),
					  SLOT( nameChanged(const QString &)));


	connect(qtarch_PushButton_1, SIGNAL(clicked()), SLOT(reject()));
	connect(resetBtn, SIGNAL(clicked()), SLOT(reset()));
}

// free up the table structures
	
HighScore::~HighScore()
{
	TableInstance *t, *t1;

	if (tables != NULL) {
		t = tables;
		while(t != NULL)
		{
			t1 = t;
			t = t -> next;
			delete t1;
		}
	}
	tables = NULL;
}



// return a pointer to a linked list of table entries from the
// saved hi score tables file. If no such file exists then
// return a single default hiscore table.

void HighScore::loadTables() {
	char buff[1024];

	// open the file, on error set up the default table
	FILE *fp = fopen( QFile::encodeName(highScoreFile()), "r");
	if (fp == NULL) 
		goto error;	

	// check magic
 	fscanf(fp, "%1023s\n", buff);
	if (highScoreMagic1_1 != buff) {
		goto error;	
	}

	int num;
	fscanf(fp, "%d\n", &num);

	for (int p=0; p<num; p++) {
		TableInstance *t = new TableInstance;
		t->next = tables;


		tables = t;

		fgets(buff, sizeof(buff), fp);
		if (buff[strlen(buff)-1] == '\n')
			buff[strlen(buff)-1] = '\0';
		t->name = buff;
		combo->insertItem(t->name);
		setComboTo(t->name);
		for (int e=0; e<numScores; e++) {
			fscanf(fp, "%ld\n", &(t->entries[e].score)); 
			fscanf(fp, "%ld\n", &(t->entries[e].elapsed));
			fscanf(fp, "%ld\n", &(t->entries[e].board));
			fgets(buff, sizeof(buff), fp);
			if (buff[strlen(buff)-1] == '\n')
				buff[strlen(buff)-1] = '\0';
			t->entries[e].name=QString::fromUtf8(buff,-1);
		}
	}	


	fclose(fp);
	return;

	
error:
	selectTable("default");
	saveTables();
	return;
}

void HighScore::saveTables() {

	TableInstance *p;
	int	      num = 0;

	// Nothing to do
	if (tables == NULL)
		return;


	// open the outrput file, with naff error handling
	FILE *fp = fopen( QFile::encodeName(highScoreFile()), "w");
	if (fp == NULL)
		return;
	
	// count up the number of tables to save
 	for (p=tables; p != NULL; p = p->next)
		num++;

	// output the file magic
	fprintf(fp,"%s\n", highScoreMagic1_1.utf8().data());
	
	// output the count of tables to process
	fprintf(fp, "%d\n", num);

	// output each table
	for (p=tables; p != NULL; p = p->next) {
		fprintf(fp, "%s\n", p->name.utf8().data());
		for (int e=0; e<numScores; e++) {
			fprintf(fp,"%ld\n%ld\n%ld\n%s\n", 
				p->entries[e].score,
				p->entries[e].elapsed,
				p->entries[e].board,
				p->entries[e].name.utf8().data());
		}
	}
	fclose(fp);	

}

// traverse the list of hi score tables and set the
// current table to the specified board. Create it if it does not 
// exist.

void HighScore::selectTable(const QString &board) {

	TableInstance *pos = tables;


	while (pos != NULL) {
		if (pos->name == board)
			break;
		pos = pos->next;
	}	



	if (pos == NULL) {
		// not found, add new board to the front of the list
	        TableInstance *n = new TableInstance;
        	n->next = tables;
       	 	n->name = board; 



        	for (int p =0; p < numScores; p ++) {
                	n->entries[p].name = defNames[p];
                	n->entries[p].score = defScores[p];
                	n->entries[p].board = 928364243l+(p *3); 
			n->entries[p].elapsed = defTimes[p];
        	}
		tables = n;
		currTable = n;
		combo->insertItem(board);
		setComboTo(board);
		return;
	}
	currTable = pos;
	setComboTo(board);
	return;
}



void HighScore::addRow(int num) {
	QFont tmp;

	// game number
	numbersWidgets[num] = new QLabel( this);
	numbersWidgets[num]->setGeometry( 10, 75+(num*30), 30, 30 );
	numbersWidgets[num]->setFrameStyle( 50 );
	numbersWidgets[num]->setAlignment( AlignRight | AlignVCenter );
	numbersWidgets[num]->setNum(num+1);


	// name
	namesWidgets[num] = new QLabel( this);
	namesWidgets[num]->setGeometry( 40, 75+(num*30), 150, 30 );
	namesWidgets[num]->setFrameStyle( 50 );
	namesWidgets[num]->setAlignment( 289 );
	
	// board 
	boardWidgets[num] = new QLabel( this);
	boardWidgets[num]->setGeometry( 190, 75+(num*30), 80, 30 );
	boardWidgets[num]->setFrameStyle( 50 );
	boardWidgets[num]->setAlignment( 289 );

	// score
	scoresWidgets[num] = new QLabel( this);
	scoresWidgets[num]->setGeometry( 270, 75+(num*30), 70, 30 );
	scoresWidgets[num]->setFrameStyle( 50 );
	tmp = scoresWidgets[num]->font();
	tmp.setItalic(true);
	scoresWidgets[num]->setFont(tmp);

        // elapsed time
        elapsedWidgets[num] = new QLabel( this);
        elapsedWidgets[num]->setGeometry( 270+70, 75+(num*30), 70, 30 );
        elapsedWidgets[num]->setFrameStyle( 50 );
        tmp = elapsedWidgets[num]->font();
        tmp.setItalic(true);
        elapsedWidgets[num]->setFont(tmp);    

	
}

void HighScore::copyTableToScreen(const QString &name) {
	char buff[256];
	QString base;
	getBoardName(name, base);
	selectTable(base);
	for (int p=0; p<numScores;p++) {
		scoresWidgets[p]->setNum((int)currTable->entries[p].score);
		namesWidgets[p]->setText(currTable->entries[p].name);
		boardWidgets[p]->setNum((int)currTable->entries[p].board);

                int e = currTable->entries[p].elapsed;
                int s = e % 60;
                e = e-s;
                int m = (e % (60*60)) / 60;
                e = e - (e % (60*60));
                int h = (e % (60*60*60)) / (60*60);
                sprintf(buff, "%2.2d:%2.2d:%2.2d", h, m , s);
                elapsedWidgets[p]->setText(buff);   

	}
	repaint(false);
}

int HighScore::exec(QString &layout) {
	copyTableToScreen(layout);	
	return(QDialog::exec());
}

void HighScore::checkHighScore(int s, int e, long gameNum, QString &name) {
	int pos;

	QString board;
	getBoardName(name, board);

	// make this board name the current one!
	// creates it if it does not exist
	selectTable(board);

	
	for (pos=0; pos <numScores; pos++) {
		if (s > currTable->entries[pos].score) {
			break;
		}
	}
	if (pos >= numScores) {
		return;
	}
	for (int move= numScores-1; move >pos; move--) {
	    currTable->entries[move].score = currTable->entries[move-1].score;
	    currTable->entries[move].name = currTable->entries[move-1].name;
	    currTable->entries[move].board = currTable->entries[move-1].board;
	    currTable->entries[move].elapsed = currTable->entries[move-1].elapsed;
	}

	currTable->entries[pos].score = s;
	currTable->entries[pos].board = gameNum;
	currTable->entries[pos].elapsed = e;

	lineEdit->setEnabled(true);
	lineEdit->setGeometry( 40, 75+(pos*30), 150, 30 ); 	
	lineEdit->setFocus();
	lineEdit->setText("");
	selectedLine = pos;
	nameChanged("");

	// no board change when entering data
	combo->setEnabled(false);
	exec(board);
	combo->setEnabled(true);

	selectedLine = -1;
	lineEdit->setGeometry( 40, 75+(20*30), 150, 30);
	lineEdit->setEnabled(false);
	
	// sync the hiscore table to disk now
	saveTables();
	
}

void HighScore::nameChanged(const QString &s) {

	if (selectedLine == -1)
		return;
		
	if (s.isEmpty())
		currTable->entries[selectedLine].name = 
			i18n("Anonymous");
	else
		currTable->entries[selectedLine].name = s;
}


void HighScore::getBoardName(QString in, QString &out) {

            QFileInfo fi( in );
            out = fi.baseName();
}

void HighScore::setComboTo(const QString &to) {
	for (int p=0; p<combo->count(); p++) {
		if (combo->text(p) == to)
			combo->setCurrentItem(p);
	}
}


void HighScore::selectionChanged(int ) {
	copyTableToScreen(combo->currentText());

}


// reset the high score table. Caution the user
// before acting


void HighScore::reset() {

        int res=KMessageBox::warningContinueCancel(this,
                        i18n("Resetting the high scores will "
                                           "remove all high score entries "
                                           "both in memory and on disk. Do "
					   "you wish to proceed?"),
                        		   i18n("Reset High Scores"),i18n("Reset"));
        if (res != KMessageBox::Continue)
                return ;   

	// delete the file
	res = unlink( QFile::encodeName(highScoreFile()));
	
	// wipe ou the in memory list of tables	
	TableInstance *t, *d;

	if (tables != NULL) {
	    t = tables;
	    while (t != NULL) {
		d = t;
		t = t->next;
		d->next=0;
		delete d;
	    }

	}
	
	// set the list empty
	tables = NULL;
	currTable=NULL;	

	// clear out the combobox
	combo->clear();
	
	// stick in a default
	selectTable("default");

	// make sure tha the on screen data does not
	// point to deleted data
	copyTableToScreen("default");
}

QString &HighScore::highScoreFile() {
	return filename;

} 
