/**********************************************************************

	--- Qt Architect generated file ---

	File: PrefsDlg.h
	Last generated: Fri Jan 22 19:24:54 1999

 *********************************************************************/

#ifndef PrefsDlg_included
#define PrefsDlg_included



#include <qdialog.h>

class QRadioButton;
class QCheckBox;

class PrefsDlgData : public QDialog
{
    Q_OBJECT
	    
public:
		    
   PrefsDlgData
   (
            QWidget* parent = NULL,
            const char* name = NULL
   );
			
   virtual ~PrefsDlgData();
			    
public slots:
				    
				    
protected slots:
				    
   virtual void acceptClicked();
   virtual void okClicked();
		
protected:
   QRadioButton* dlgTileBg;
   QRadioButton* dlgScaleBg;
   QCheckBox* dlgShowShadows;
   QCheckBox* dlgShowRemoved;
   QCheckBox* dlgSavePrefs;
   QCheckBox* dlgShowStatus;
   QCheckBox* dlgMiniTiles;
							    
};                     


class PrefsDlg : public PrefsDlgData
{
    Q_OBJECT

public:

    PrefsDlg
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~PrefsDlg();

    void initialise(void);

    signals:
	void boardRedraw(bool);
	void statusBar(int); 
	void backgroundModeChanged(void);
	void showRemovedChanged(void);
	void tileSizeChanged(void);

protected slots:

    virtual void acceptClicked();
    virtual void okClicked();        

protected:
    void updatePreferences(void);
private:
    int oStatus;
    int oShadows;
    int oSave;
    int oRemoved;
    int oScale;
    int oMini;
 
};
#endif // PrefsDlg_included
