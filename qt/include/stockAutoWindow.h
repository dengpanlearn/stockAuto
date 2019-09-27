/*
*stockAutoWindow.h
*/

#ifndef __STOCK_AUTO_WINDOW_H__
#define	__STOCK_AUTO_WINDOW_H__


#include <QtWidgets/QMainWindow>

class CQtServerTask;
class CQtExitAgent;
class QPushButton;
class CQtStockAgent;


class CStockAutoWindow : public QMainWindow
{
    Q_OBJECT

public:
    CStockAutoWindow(QWidget *parent = Q_NULLPTR);

private:
	void OnInit();
	void RetranlateUi();

	BOOL OnInitQtServerAndAgent();

private slots:
	void OnNotifyAutoManagerStep();
	void OnNotifyStockTrace();

private:
	QPushButton* m_pBtn;
	CQtServerTask*	m_pServerTask;
	CQtExitAgent* m_pExitAgent;
	CQtStockAgent*	m_pStockAgent;
};

#endif // !__STOCK_AUTO_WINDOW_H__
