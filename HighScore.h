
#ifndef HighScore_included
#define HighScore_included

#include <qdialog.h>


class QLineEdit;
class QComboBox;
class QLabel;

const int numScores = 10;

typedef struct HiScoreEntry  {
	QString name;
	long	board;
	long 	score;
	long elapsed;
	
};

typedef struct TableInstance  {
	QString name;
	HiScoreEntry entries[numScores];
	TableInstance *next;	
};


class HighScore : public QDialog
{
    Q_OBJECT

public:

    HighScore
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~HighScore();

	int exec(QString &layout);


    void checkHighScore(int score, int elapsed, long game, QString &board);
public slots:
	void selectionChanged(int);

protected slots:
	void nameChanged(const QString &s);
	void reset();
private:
	void 		addRow(int num);	// generate one table row 
	void		loadTables();	// initialise from saved
	void		saveTables();	// save to disc.
	void		getBoardName(QString in, QString &out);
	void 		selectTable(const QString &name);
	void		setComboTo(const QString &to);
	void		copyTableToScreen(const QString &name);
	QString		&highScoreFile();
	
	int 		selectedLine;
	QLineEdit   	*lineEdit;
        QLabel*		numbersWidgets[numScores];
	QLabel* 	boardWidgets[numScores];
        QLabel*     	namesWidgets[numScores];
        QLabel* 	scoresWidgets[numScores]; 
	QLabel*       elapsedWidgets[numScores]; 
	QComboBox*	combo;
	QString 	filename;

	TableInstance *tables;		
	TableInstance *currTable;		
};

#endif // HighScore_included
