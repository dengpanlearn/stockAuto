/*
*stockAutoWindow.cpp
*/
#include <global.h>
#include "../include/qtServerTask.h"
#include "../include/qtStockTraceDef.h"
#include "../include/qtObjectAgent.h"
#include "../include/qtStockAgent.h"
#include "../include/stockAutoWindow.h"

CStockAutoWindow::CStockAutoWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_pServerTask = NULL;
	m_pExitAgent = NULL;
	OnInit();
	RetranlateUi();
}

void CStockAutoWindow::OnInit()
{
	OnInitQtServerAndAgent();

}

BOOL CStockAutoWindow:: OnInitQtServerAndAgent()
{
	m_pServerTask = new CQtServerTask();
	if (m_pServerTask == NULL)
		return FALSE;

	do
	{
		CQtExitAgent* pExitAgent = new CQtExitAgent();
		if (pExitAgent == NULL)
			break;

		CQtStockAgent* pStockAgent = new CQtStockAgent();
		if (pStockAgent == NULL)
			break;

		m_pServerTask->start();

		if (!pExitAgent->Create(m_pServerTask))
			break;

		if (!pStockAgent->Create(m_pServerTask, QT_STOCK_AGENT_UPDATE_TIMEOUT))
			break;

		connect(m_pServerTask, SIGNAL(finished()), m_pServerTask, SLOT(deleteLater()));

		m_pExitAgent = pExitAgent;
		m_pStockAgent = pStockAgent;
		return TRUE;

	} while (FALSE);

	if (m_pServerTask->isRunning())
		m_pServerTask->quit();

	delete m_pServerTask;
	m_pServerTask = NULL;

	if (m_pStockAgent != NULL)
	{
		delete m_pStockAgent;
		m_pStockAgent = NULL;
	}

	if (m_pExitAgent != NULL)
	{
		delete m_pExitAgent;
		m_pExitAgent = NULL;
	}

	return FALSE;
}

void CStockAutoWindow::RetranlateUi()
{
	if (m_pStockAgent != NULL)
	{
		connect(m_pStockAgent, SIGNAL(NotifyUiManagerStep()), this, SLOT(OnNotifyAutoManagerStep()));
		connect(m_pStockAgent, SIGNAL(NotifyUiStockTrace()), this, SLOT(OnNotifyStockTrace()));
	}


}

void CStockAutoWindow::OnNotifyAutoManagerStep()
{

}

void CStockAutoWindow::OnNotifyStockTrace()
{

}