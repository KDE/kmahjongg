/* -------------------------------------------------------------------------
   -- kmahjongg timer. Based on a slightly modified verion of the QT demo --
   -- program dclock 
   This file is part of an example program for Qt.  This example
   program may be used, distributed and modified without limitation.
   ------------------------------------------------------------------------- */

/*
    dclock Copyright (C) 1992-1998 Troll Tech AS. qt@trolltech.com

    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini  <mauricio@tabuleiro.com>

    KMahjongg is free software; you can redistribute it and/or modify
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

#ifndef KM_GAME_TIMER 
#define KM_GAME_TIMER 

#include <QDateTime>

enum TimerMode {running = -53 , stopped= -54 , paused = -55};

class GameTimer: public QObject 
{
    Q_OBJECT
public:
    GameTimer();

    int toInt(); 
    QString toString() {return theTimer.toString();}	
    void fromString(const char *);

protected:					// event handlers
    void	timerEvent( QTimerEvent * );
 
public slots:
    void start();
    void stop();
    void pause();
    signals:
        void displayTime (QString& );


private slots:					// internal slots
    void	showTime();

private:					// internal data
    bool	showingColon;
    QTime	theTimer;
    TimerMode   timerMode;
};


#endif 
