/*
 * Based upon the QT example dclock 
 */

#include <stdio.h>
#include "GameTimer.h"
#include "GameTimer.moc"


//
// Constructs a GameTimer widget with a parent and a name.
//

GameTimer::GameTimer( QWidget *parent, const char *name )
    : QLCDNumber( parent, name )
{
    showingColon = false;
    setNumDigits(7);	
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFrameStyle(QFrame::NoFrame);
    timerMode = stopped;
    showTime();					// display the current time1
    startTimer( 500 );		// 1/2 second timer events
}

// QObject timer call back implementation
void GameTimer::timerEvent( QTimerEvent * )
{
    if (timerMode == running)
    	theTimer=theTimer.addMSecs(500);	
    showTime();
}


//
// Shows the current time in the internal lcd widget.
//

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

    if ( !showingColon )
	s[2] = s[5] = ' ';
    display( s );				// set LCD number/text
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
