/*
*stockAutoWindow.cpp
*/
#include <global.h>
#include <qstyle.h>
#include <qfile.h>
#include <qapplication.h>
#include <qsplitter.h>
#include <stockCalcDef.h>
#include <qtextcodec.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qaction.h>
#include <stockAutoManager.h>
#include "../include/stockSettingDialog.h"
#include "../include/stockHisWidget.h"
#include "../include/stockRealWidget.h"
#include "../include/stockTraceWidget.h"
#include "../include/qtWaittingDialog.h"
#include "../include/qtServerTask.h"
#include "../include/qtStockTraceDef.h"
#include "../include/qtObjectAgent.h"
#include "../include/qtStockAgent.h"
#include "../include/queryTraceRecordDialog.h"
#include "../include/stockAutoWindow.h"

extern CStockAutoManager g_autoManger;;

CStockAutoWindow::CStockAutoWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_bLoading = FALSE;
	m_pServerTask = NULL;
	m_pExitAgent = NULL;
	m_pStockAgent = NULL;
	m_pWaitDialog = NULL;
	m_pLoadingDialog = NULL;
	m_pQueryTraceRecordDialog = NULL;
	OnInit();
	RetranlateUi();
}

CStockAutoWindow::~CStockAutoWindow()
{
	if (m_pLoadingDialog != NULL)
		delete m_pLoadingDialog;

	if (m_pExitAgent != NULL)
	{
		delete m_pExitAgent;
		m_pExitAgent = NULL;
	}

	if (m_pStockAgent != NULL)
	{
		delete m_pStockAgent;
		m_pStockAgent = NULL;
	}
}

void CStockAutoWindow::UpdateAutoManagerStep(UINT traceStep, int loadProgress)
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	if (pStockAgent != NULL)
		pStockAgent->UpdateAutoManagerStep(traceStep, loadProgress);
}

BOOL CStockAutoWindow::UpdateStockTrace(char const* pStockName, char const* pStockCode, UINT updateStat)
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	if (pStockAgent != NULL)
		return pStockAgent->UpdateStockTrace(pStockName, pStockCode, updateStat);
	else
		return TRUE;
}

void CStockAutoWindow::UpdateResetTraceResult(int result)
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	if (pStockAgent != NULL)
		pStockAgent->UpdateResetTraceResult(result);
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

	m_pTraceWidget = new CStockTraceWidget(NULL,m_pExitAgent, m_pStockAgent);
	m_pRealWidget = new CStockRealWidget(NULL, m_pExitAgent, m_pStockAgent);

	m_pHisWidget = new CStockHisWidget(NULL, m_pExitAgent, m_pStockAgent);

	m_pRealWidget->setObjectName("Stock_Real_Widget");
	m_pTraceWidget->setObjectName("Stock_Trace_Widget");
	m_pHisWidget->setObjectName("Stock_His_Widget");
	QSplitter* pSplitLeft = new QSplitter(Qt::Horizontal, this);
	pSplitLeft->addWidget(m_pRealWidget);
	QSplitter* pSplitRight = new QSplitter(Qt::Horizontal, pSplitLeft);
	pSplitLeft->setStretchFactor(0, 2);
	pSplitLeft->setStretchFactor(1, 3);

	pSplitRight->addWidget(m_pTraceWidget);
	pSplitRight->addWidget(m_pHisWidget);
	pSplitRight->setStretchFactor(0, 3);
	pSplitRight->setStretchFactor(1, 2);
	this->setCentralWidget(pSplitLeft);

	m_pLoadingDialog = new CStockLoadingDialog(NULL);
	m_pLoadingDialog->setFixedSize(350, 50);
	m_pLoadingDialog->setObjectName("Stock_Loading_Dialog");
	m_pLoadingDialog->setModal(true);

	QMenuBar* pMenuBar = this->menuBar();
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QMenu* pMenu = pMenuBar->addMenu(pCodec->toUnicode(STOCK_AUTO_WINDOW_MEUNU_FUNC));
	m_pActSetting = pMenu->addAction(pCodec->toUnicode(STOCK_AUTO_WINDOW_ACTION_SETTING));
	m_pActResetTrace = pMenu->addAction(pCodec->toUnicode(STOCK_AUTO_WINDOW_ACTION_RESET_TRACE));

	QMenu* pMenuQuery = pMenuBar->addMenu(pCodec->toUnicode(STOCK_AUTO_WINDOW_MEUNU_QUERY));
	m_pActQueryTraceRecord = pMenuQuery->addAction(pCodec->toUnicode(STOCK_AUTO_WINDOW_ACTION_QUERY_TRACERECORD));
}

void CStockAutoWindow::RetranlateUi()
{
	if (m_pStockAgent != NULL)
	{
		CQtStockAgent* pStockAgent = (CQtStockAgent* )m_pStockAgent;
		connect(pStockAgent, SIGNAL(NotifyUiManagerLoadingProgress()), this, SLOT(OnNotifyAutoManagerLoadingProgress()));
		connect(this, SIGNAL(UpdateLoadingProgress(QString&, QString& )), m_pLoadingDialog, SLOT(OnUpdateLoadingProgress(QString&, QString&)));
		connect(pStockAgent, SIGNAL(NotifyResetTraceResponese()), this, SLOT(OnResetTraceResponese()));
		connect(this, SIGNAL(SignalResetTrace()), pStockAgent, SLOT(OnRequestResetTrace()));
	}

	connect(m_pTraceWidget, SIGNAL(SignalSelectStock(QString&, QString&)), m_pRealWidget, SLOT(OnSelectStock(QString&, QString&)));
	connect(m_pActSetting, SIGNAL(triggered(bool)), this, SLOT(OnEnterSettingDialog(bool)));
	connect(m_pActResetTrace, SIGNAL(triggered(bool)), this, SLOT(OnSelectResetTrace(bool)));
	connect(m_pActQueryTraceRecord, SIGNAL(triggered(bool)), this, SLOT(OnEnterQueryTraceRecordDialog(bool)));
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

		if (!pStockAgent->Create(m_pServerTask, &g_autoManger, QT_STOCK_AGENT_UPDATE_TIMEOUT))
			break;

		pStockAgent->Active();
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
	GlobalExit();

	if (m_pExitAgent != NULL)
	{
		if (m_pServerTask->isRunning())
		{
			m_pStockAgent->InActive();
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
	}
}


void CStockAutoWindow::OnNotifyAutoManagerLoadingProgress()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	QT_STOCK_LOADING_MANAGER loadingManager;

	pStockAgent->GetAutoManagerLoading(&loadingManager);

	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QString stat;
	QString progress;

	if (!m_bLoading)
	{
		m_bLoading = TRUE;
		m_pLoadingDialog->show();
	}

	switch (loadingManager.step)
	{
	default:
	case STOCK_AUTO_MANAGER_STEP_NONE:
		stat = pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_INIT);
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_INIT:
		stat = pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_LIST_LOADING);
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_UPDATING:
		stat = pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_LIST_UPDATING);
		break;

	case STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING:
		stat = pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_TRACELOG_LOADING);
		break;

	case STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING:
		stat = pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_HISKLINE_UPDATING);
		progress = QString::number(loadingManager.progress) + "%";
		break;

	case STOCK_AUTO_MANAGER_STEP_STOCK_TRACING:
		stat = pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_STOCK_TRACING);
		m_pLoadingDialog->hide();
		break;

	case STOCK_AUTO_MANAGER_STEP_ERROR:
		break;
	}


	emit UpdateLoadingProgress(stat, progress);

}

void CStockAutoWindow::OnEnterSettingDialog(bool check)
{
	CStockSettingDilaog* pSettingDialog = new CStockSettingDilaog(NULL, m_pStockAgent);
	pSettingDialog->setObjectName("Stock_Setting_Dialog");
	pSettingDialog->exec();
	delete pSettingDialog;
}

void CStockAutoWindow::OnSelectResetTrace(bool check)
{
	if (m_pWaitDialog)
		return;

	QTextCodec* pCodec = QTextCodec::codecForLocale();

	QMessageBox::StandardButton resultBtn = QMessageBox::warning(this, pCodec->toUnicode(STOCK_AUTO_WINDOW_RESET_TITLE), pCodec->toUnicode(STOCK_AUTO_WINDOW_RESET_TEXT),
		QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

	if (resultBtn == QMessageBox::Cancel)
		return;
	m_pWaitDialog = new CQtWaitting(NULL);
	emit SignalResetTrace();

	m_pWaitDialog->exec();
}

void CStockAutoWindow::OnResetTraceResponese()
{
	if (m_pWaitDialog != NULL)
	{
		m_pWaitDialog->close();
		delete m_pWaitDialog;
		m_pWaitDialog = NULL;
	}
}

void CStockAutoWindow::OnEnterQueryTraceRecordDialog(bool check)
{
	m_pQueryTraceRecordDialog = new CQueryTraceRecordDialog(NULL);
	m_pQueryTraceRecordDialog->setObjectName("Query_TraceRecord_Dialog");
	m_pQueryTraceRecordDialog->exec();
	delete m_pQueryTraceRecordDialog;
}
