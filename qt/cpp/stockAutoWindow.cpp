/*
*stockAutoWindow.cpp
*/
#include <global.h>
#include <qstyle.h>
#include <qfile.h>
#include <qapplication.h>
#include <qsplitter.h>
#include <stockCalcDef.h>
#include "../include/stockHisWidget.h"
#include "../include/stockRealWidget.h"
#include "../include/stockTraceWidget.h"
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
	m_pStockAgent = NULL;
	OnInit();
	RetranlateUi();
}

CStockAutoWindow::~CStockAutoWindow()
{

}

void CStockAutoWindow::OnInit()
{
	OnInitQtServerAndAgent();

	QFile qssFile(":/CStockAutoWindow/stockAuto.qss");
	if (qssFile.open(QFile::ReadOnly))
	{
		qApp->setStyleSheet(qssFile.readAll());
		qssFile.close();
	}

	m_pTraceWidget = new CStockTraceWidget(NULL);
	m_pRealWidget = new CStockRealWidget(NULL);

	m_pHisWidget = new CStockHisWidget(NULL);

	m_pRealWidget->setObjectName("Stock_Real_Widget");
	QSplitter* pSplitLeft = new QSplitter(Qt::Horizontal, this);
	pSplitLeft->addWidget(m_pRealWidget);
	QSplitter* pSplitRight = new QSplitter(Qt::Horizontal, pSplitLeft);

	
	
	pSplitRight->addWidget(m_pTraceWidget);
	pSplitRight->addWidget(m_pHisWidget);
	this->setCentralWidget(pSplitLeft);
}

void CStockAutoWindow::RetranlateUi()
{
	if (m_pStockAgent != NULL)
	{
		connect(m_pStockAgent, SIGNAL(NotifyUiManagerStep()), this, SLOT(OnNotifyAutoManagerStep()));
		connect(m_pStockAgent, SIGNAL(NotifyUiStockTrace()), this, SLOT(OnNotifyStockTrace()));
	}


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

void CStockAutoWindow::closeEvent(QCloseEvent * event)
{
	if (m_pExitAgent != NULL)
	{
		if (m_pServerTask->isRunning())
		{
			m_pExitAgent->Active();
			QThread::msleep(QT_STOCK_SERVER_EXIT_WAIT_TIMEOUT);
			if (m_pServerTask->isRunning())
				m_pServerTask->quit();
		}

		delete m_pExitAgent;
		m_pExitAgent = NULL;

		delete m_pStockAgent;
		m_pStockAgent = NULL;
	}
}


void CStockAutoWindow::OnNotifyAutoManagerStep()
{

}

void CStockAutoWindow::OnNotifyStockTrace()
{

}