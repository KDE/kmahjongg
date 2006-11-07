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

#include <stdio.h>
#include <QtDebug>
#include "GameTimer.h"
#include "GameTimer.moc"


GameTimer::GameTimer( )
{
    showingColon = false;
    timerMode = stopped;
    showTime();	
    startTimer( 500 );	
}

// QObject timer call back implementation
void GameTimer::timerEvent( QTimerEvent * )
{
    if (timerMode == running)
    	theTimer=theTimer.addMSecs(500);	
    showTime();
}

void GameTimer::showTime()
{
    QString s;
    showingColon = !showingColon;		// toggle/blink colon

    switch(timerMode) {
	case paused:
	case running:
    	    s = theTimer.toString(); 
	break;
	case stopped:
	    s = "00:00:00";
	break;
    }

    if ( !showingColon ) s[2] = s[5] = ' ';
    emit displayTime(s);
}

void GameTimer::start() {
	theTimer.setHMS(0,0,0);
	timerMode = running;
}

void GameTimer::fromString(const char*tim) {
	int h,m,s;
	sscanf(tim, "%2d:%2d:%2d\n", &h, &m, &s);
	theTimer.setHMS(h,m,s);
	timerMode = running;
}

void GameTimer::stop() {
	timerMode = stopped;
}

void GameTimer::pause() {
	if (timerMode == stopped)
		return;

	if (timerMode == paused) {
		timerMode = running;
	} else { 
		timerMode = paused; 
	}
}

int GameTimer::toInt() {

	return (theTimer.second()+
		theTimer.minute()*60+
		theTimer.hour()*360);
}
