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

#ifndef HighScore_included
#define HighScore_included

#include <qdialog.h>


class QLineEdit;
class QComboBox;
class QLabel;

const int numScores = 10;

struct HiScoreEntry{
	QString name;
	long	board;
	long 	score;
	long elapsed;
	
};

struct TableInstance  {
	QString name;
	HiScoreEntry entries[numScores];
	TableInstance *next;	
};


class HighScore : public QDialog
{
    Q_OBJECT

public:

    HighScore ( QWidget* parent = 0);

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
	void		getBoardName(const QString &in, QString &out);
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
