/*
*stockAutoWindow.cpp
*/
#include <global.h>
#include <qstyle.h>
#include <qfile.h>
#include <qapplication.h>
#include <qsplitter.h>
#include <stockCalcDef.h>
#include <stockAutoManager.h>
#include "../include/stockHisWidget.h"
#include "../include/stockRealWidget.h"
#include "../include/stockTraceWidget.h"
#include "../include/qtServerTask.h"
#include "../include/qtStockTraceDef.h"
#include "../include/qtObjectAgent.h"
#include "../include/qtStockAgent.h"
#include "../include/stockAutoWindow.h"

extern CStockAutoManager g_autoManger;;

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

	m_pHisWidget = new CStockHisWidget(NULL, m_pExitAgent, m_pStockAgent);

	m_pRealWidget->setObjectName("Stock_Real_Widget");
	m_pTraceWidget->setObjectName("Stock_Trace_Widget");
	m_pHisWidget->setObjectName("Stock_His_Widget");
	QSplitter* pSplitLeft = new QSplitter(Qt::Horizontal, this);
	pSplitLeft->addWidget(m_pRealWidget);
	QSplitter* pSplitRight = new QSplitter(Qt::Horizontal, pSplitLeft);
	pSplitLeft->setStretchFactor(0, 1);
	pSplitLeft->setStretchFactor(1, 4);

	pSplitRight->addWidget(m_pTraceWidget);
	pSplitRight->addWidget(m_pHisWidget);
	pSplitRight->setStretchFactor(0, 4);
	pSplitRight->setStretchFactor(1, 3);
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

		if (!pStockAgent->Create(m_pServerTask, g_autoManger.GetTaskData(), QT_STOCK_AGENT_UPDATE_TIMEOUT))
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
			int waitTimes = QT_STOCK_SERVER_EXIT_WAIT_TIMEOUT / STOCK_AUTO_MANAGER_EXIT_CHECK_TIMEOUT;
			while (waitTimes-- > 0)
			{
				if (!m_pServerTask->isRunning())
					break;

				QThread::msleep(STOCK_AUTO_MANAGER_EXIT_CHECK_TIMEOUT);
			}

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