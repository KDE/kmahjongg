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

#include <KDialog>


class QLineEdit;
class QComboBox;
class QLabel;

const int numScores = 10;

/**
 * @short This struct implements
 */
struct HiScoreEntry{
	QString name; /**< Member Description */
	long	board;/**< Member Description */
	long 	score;/**< Member Description */
	long elapsed; /**< Member Description */
	
};

/**
 * @short This struct implements
 */
struct TableInstance  {
	QString name; /**< Member Description */
	HiScoreEntry entries[numScores]; /**< Member Description @see HiScoreEntry */
	TableInstance *next; /**< Member Description @see TableInstance */
};

/**
 * @short This class implements
 * 
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class HighScore : public KDialog
{
    Q_OBJECT

public:
    /**
     * Default Constructor Description */
    explicit HighScore ( QWidget* parent = 0);
    /**
     * Virtual Destructor Description */
    virtual ~HighScore();
    /**
     * Method Description
     *
     * @param layout
     * @return int blah blah
     */
	int exec(QString &layout);
    /**
     * Method Description
     * 
     * @param score blah blah
     * @param elapsed
     * @param game
     * @param board
     */
    void checkHighScore(int score, int elapsed, long game, QString &board);
public slots:
    /**
     * Slot Description 
     */
	void selectionChanged(int);

protected slots:
    /**
     * Slot Description 
     * @param s
     */
	void nameChanged(const QString &s);
    /**
     * Slot Description 
     */
	void reset();
private:
    /**
     * generate one table row  */
	void 		addRow(int num);
    /**
     * initialise from saved */
	void		loadTables();
    /**
     * save to disc */	
	void		saveTables();
    /**
     * Method Description 
     * @param in
     * @param out
     */
	void		getBoardName(const QString &in, QString &out);
    /**
     * Method Description 
     * @param name
     */
	void 		selectTable(const QString &name);
    /**
     * Method Description 
     * @param to
     */
	void		setComboTo(const QString &to);
    /**
     * Method Description
     * @param name
     */
	void		copyTableToScreen(const QString &name);
    /**
     * Method Description 
     * @return QString filename?
     */
	QString		&highScoreFile();
	
	int 		selectedLine; /**< Member Description */
	QLineEdit   	*lineEdit;/**< Member Description */
    
    QLabel*		numbersWidgets[numScores];  /**< Member Description */
	QLabel* 	boardWidgets[numScores];    /**< Member Description */
    QLabel*    	namesWidgets[numScores];    /**< Member Description */
    QLabel* 	scoresWidgets[numScores];   /**< Member Description */
	QLabel*     elapsedWidgets[numScores];  /**< Member Description */
	QComboBox*	combo;      /**< Member Description */
	QString 	filename;   /**< Member Description */

	TableInstance *tables;  /**< Member Description */
	TableInstance *currTable;/**< Member Description */
};

#endif // HighScore_included
