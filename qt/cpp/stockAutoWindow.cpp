
#include "../include/stockAutoWindow.h"

CStockAutoWindow::CStockAutoWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_pBtn = new QPushButton("hello");

	m_pServerTask = new CQtServerTask();
	m_pServerTask->start();

	m_pExitAgent = new CQtExitAgent(NULL);


	if (!m_pExitAgent->Create(m_pServerTask))
	{
		
	}
	connect(m_pBtn, SIGNAL(clicked(bool)), this, SLOT(OnBtn(bool)));
	this->setCentralWidget(m_pBtn);
}


void CStockAutoWindow::OnBtn(bool)
{
	m_pExitAgent->Active();
	while (!m_pServerTask->isFinished())
		Sleep(1);
	delete m_pExitAgent;
}