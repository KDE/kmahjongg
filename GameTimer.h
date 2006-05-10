/* -------------------------------------------------------------------------
   -- kmahjongg timer. Based on a slightly modified verion of the QT demo --
   -- program dclock. Copyright as shown below.                           --
   ------------------------------------------------------------------------- */

/****************************************************************************
** $Id$
**
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef KM_GAME_TIMER 
#define KM_GAME_TIMER 

#include <qlcdnumber.h>
#include <QDateTime>

enum TimerMode {running = -53 , stopped= -54 , paused = -55};

class GameTimer: public QLCDNumber 
{
    Q_OBJECT
public:
    GameTimer( QWidget *parent=0 );

    int toInt(); 
    QString toString() {return theTimer.toString();};	
    void fromString(const char *);

protected:					// event handlers
    void	timerEvent( QTimerEvent * );
 
public slots:
    void start();
    void stop();
    void pause();


private slots:					// internal slots
    void	showTime();

private:					// internal data
    bool	showingColon;
    QTime	theTimer;
    TimerMode   timerMode;
};


#endif 
