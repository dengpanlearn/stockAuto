/*
*qtWaittingDialog.h
*/

#ifndef __QT_WAITTING_DIALOG_H__
#define	__QT_WAITTING_DIALOG_H__

#include <qwidget.h>
#include <qdialog.h>

class QLabel;
class QMovie;

class CQtWaitting :public QDialog 
{
	Q_OBJECT
public:
	CQtWaitting(QWidget* parent=NULL);
	~CQtWaitting(){}

private:
	void Onit();
	void RetranslateUi();


private:
	QLabel*		m_pLabWait;
	QMovie*		m_pMovieWait;
};
#endif // !__QT_WAITTING_DIALOG_H__
