#pragma once
#include <qthread.h>
#include <qtObjectAgent.h>
#include <qtServerTask.h>
#include <QtWidgets/QMainWindow>
#include <qpushbutton.h>

class CStockAutoWindow : public QMainWindow
{
    Q_OBJECT

public:
    CStockAutoWindow(QWidget *parent = Q_NULLPTR);

	private slots:
	void OnBtn(bool);
private:
	QPushButton* m_pBtn;
	CQtServerTask*	m_pServerTask;
	CQtExitAgent* m_pExitAgent;
};
