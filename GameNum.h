
#ifndef __GAME__NUM 
#define __GAME__NUM

#include <qdialog.h>


class QLabel;
class QLineEdit;
class QPushButton;

class GameNum : public QDialog
{
    Q_OBJECT

public:

    GameNum
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~GameNum();

	int exec();


    void checkGameNum(int score);
public:
    	int getNumber(void);
public slots:
	void numChanged(const QString &s);
protected:
	void checkOK(const QString &s);
	

private:
	QLineEdit   *lineEdit;
        QLabel*     label; 
	QPushButton *okBtn;
	QPushButton *canBtn;
		
};

#endif // GameNum_included
