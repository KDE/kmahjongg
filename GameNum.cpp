#include "GameNum.h"
#include "GameNum.moc"
#include <ctype.h>
#include <stdlib.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>

#include <klocale.h>
#include <kglobal.h>
#include <kapp.h>

GameNum::GameNum
(
	QWidget* parent,
	const char* name
)
	:
	QDialog( parent, name, TRUE, 0 )
{

	label= new QLabel( this, "Label" );
	label->setGeometry( 10, 10, 200, 30 );
	label->setText( i18n("Enter game number") );

	// Line edit for numeric entry
	lineEdit = new QLineEdit(this, "");
	lineEdit->setGeometry( 10, 45, 200, 30 );
	lineEdit->setFocusPolicy(QWidget::StrongFocus);
	lineEdit->setFrame(true);
	lineEdit->setEchoMode(QLineEdit::Normal);
	lineEdit->setText("");


	// OK  button	
	okBtn = new QPushButton( this, "PushButton_1" );
	okBtn->setGeometry( 10, 90, 95, 30 );
	okBtn->setFocusPolicy( QWidget::TabFocus );
	okBtn->setText(i18n( "&OK" ));
	okBtn->setAutoRepeat( FALSE );
        okBtn->setAutoResize( FALSE );
        okBtn->setDefault(true);

	//  cancel button	
	canBtn = new QPushButton( this, "PushButton_1" );
	canBtn->setGeometry( 115, 90, 95, 30 );
	canBtn->setFocusPolicy( QWidget::TabFocus );
	canBtn->setText(i18n( "&Cancel" ));
	canBtn->setAutoRepeat( FALSE );
	canBtn->setAutoResize( FALSE );


	setCaption(kapp->makeStdCaption(i18n("New game")));

	resize( 220,130 );
	setFixedSize(220,130);



	connect(lineEdit, SIGNAL( textChanged(const QString &)),
					  SLOT( numChanged(const QString &)));


	connect(canBtn, SIGNAL(clicked()), SLOT(reject()));
	connect(okBtn, SIGNAL(clicked()), SLOT(accept()));
}

int GameNum::exec() {
	checkOK(lineEdit->text());
	return(QDialog::exec());
}



int GameNum::getNumber(void)
{
	return((int)KGlobal::locale()->readNumber(lineEdit->text()));
}


void GameNum::checkOK(const QString &s)
{
	bool ok = true;
        KGlobal::locale()->readNumber(s, &ok);

	okBtn->setEnabled(ok);
}

void GameNum::numChanged(const QString &s)
{
	checkOK(s);
}

GameNum::~GameNum()
{
}
