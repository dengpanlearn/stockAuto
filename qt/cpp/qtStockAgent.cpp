/*
*qtStockAgent.cpp
*
*/

#include <stockCalcDef.h>
#include <multiEventsTask.h>
#include "../include/qtStockTraceDef.h"
#include "../include/qtGlobal.h"
#include "../include/qtStockAgent.h"

CQtStockAgent::CQtStockAgent(QObject* parent): CQtTimeAgent(parent)
{
	m_updateCmd = 0;
	memset(&m_loadingManager, 0, sizeof(m_loadingManager));
	dllInit(&m_listTraceLog);
}

CQtStockAgent::~CQtStockAgent()
{
	Close();
}

BOOL CQtStockAgent::Create(QThread* pServerObj, CMultiEventsTask* pDataTask, int timeoutMs)
{
	m_pHisKLineQueryJob = (QT_STOCK_HISKLINE_QUERY_JOB*)calloc(1, sizeof(QT_STOCK_HISKLINE_QUERY_JOB));
	if (m_pHisKLineQueryJob == NULL)
		return FALSE;

	m_pDataTask = pDataTask;
	return CQtTimeAgent::Create(pServerObj, timeoutMs);
}

void CQtStockAgent::Close()
{
	if (m_pHisKLineQueryJob != NULL)
	{
		free(m_pHisKLineQueryJob);
		m_pHisKLineQueryJob = NULL;
	}
}

void CQtStockAgent::UpdateAutoManagerStep(UINT traceStep, int loadProgress)
{
	CSingleLock lock(&m_cs, TRUE);
	m_updateCmd |= QT_STOCK_AGENT_UPDATE_MANAGER_STEP;
	m_loadingManager.step = traceStep;
	m_loadingManager.progress = loadProgress;
}

BOOL CQtStockAgent::UpdateStockTrace(char const* pStockName, STOCK_MANAGER_TRACE_LOG* pTraceLog, UINT updateStat)
{
	QT_STOCK_TRACE_LOG_NODE* pTraceLogNode = (QT_STOCK_TRACE_LOG_NODE*)calloc(sizeof(QT_STOCK_TRACE_LOG_NODE), 1);
	if (pTraceLogNode == NULL)
		return FALSE;

	pTraceLogNode->traceLogVal.stat = updateStat;
	strncpy(pTraceLogNode->traceLogVal.stockName, pStockName, STOCK_CODE_NAME_MAX);
	memcpy(&pTraceLogNode->traceLogVal.traceLog, pTraceLog, sizeof(pTraceLog));

	CSingleLock lock(&m_cs, TRUE);
	m_updateCmd |= QT_STOCK_AGENT_UPDATE_STOCK_TRACE;
	dllInsert(&m_listTraceLog, NULL, &pTraceLogNode->node);

	return TRUE;
}


void CQtStockAgent::GetAutoManagerLoading(QT_STOCK_LOADING_MANAGER* pLoadingInfo)
{
	CSingleLock lock(&m_cs);
	memcpy(pLoadingInfo, &m_loadingManager, sizeof(QT_STOCK_LOADING_MANAGER));
}

BOOL CQtStockAgent::GetAckStockTrace(QT_STOCK_TRACE_LOG* pTraceLog)
{
	union
	{
		QT_STOCK_TRACE_LOG_NODE* pTraceLogNode;
		DL_NODE*		pNode;
	};
	
	CSingleLock lock(&m_cs, TRUE);
	pNode = dllGet(&m_listTraceLog);
	if (pNode == NULL)
		return FALSE;

	memcpy(pTraceLog, &pTraceLogNode->traceLogVal, sizeof(QT_STOCK_TRACE_LOG));
	free(pTraceLogNode);
	return TRUE;
}

int	CQtStockAgent:: GetStockHisKLine(int count, int offset, STOCK_CALC_TRACE_KLINE* pKLine)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_pHisKLineQueryJob->jobRsult < 0)
		return -1;

	if (offset >= m_pHisKLineQueryJob->hisCounts)
		return 0;

	int left = m_pHisKLineQueryJob->hisCounts-offset;
	if (left > count)
		left = count;

	memcpy(pKLine, m_pHisKLineQueryJob->hisKLine, sizeof(STOCK_CALC_TRACE_KLINE)*left);

	return left;
}

BOOL CQtStockAgent::QtTaskEventComplete(UINT cmd, int result, void* param, int paramLen)
{
	union
	{
		QT_STOCK_HISKLINE_QUERY_PARAM* pQueryHisKLine;
	};

	pQueryHisKLine = (QT_STOCK_HISKLINE_QUERY_PARAM*)param;

	switch (cmd)
	{
	default:
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_HISKLINE:
		pQueryHisKLine->pStockAgent->OnHisKLineQueryResponse(result, pQueryHisKLine);
		break;
	}


	return TRUE;
}

void CQtStockAgent::OnHisKLineQueryResponse(int result, QT_STOCK_HISKLINE_QUERY_PARAM* pHisKLine)
{
	CSingleLock lock(&m_cs, TRUE);
	m_pHisKLineQueryJob->jobRsult = result;
	m_pHisKLineQueryJob->hisCounts = result;
	m_updateCmd |= QT_STOCK_AGENT_QUERY_HISKLINE_RESPONESE;
}

void CQtStockAgent::OnGetQueryHisKLine(QString& code)
{
	m_pHisKLineQueryJob->hisCounts = 0;
	m_pHisKLineQueryJob->jobRsult = 0;
	QString2Char(code, m_pHisKLineQueryJob->code);

	QT_STOCK_HISKLINE_QUERY_PARAM* pQueryParam = (QT_STOCK_HISKLINE_QUERY_PARAM* )m_pDataTask->AllocPktByEvent(STOCK_QT_EVENT_QUERY_STOCK_HISKLINE, sizeof(QT_STOCK_HISKLINE_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
		CSingleLock lock(&m_cs, TRUE);
		m_pHisKLineQueryJob->jobRsult = -1;
		m_updateCmd |= QT_STOCK_AGENT_QUERY_HISKLINE_RESPONESE;
	
		return;
	}

	memcpy(pQueryParam->code, m_pHisKLineQueryJob->code, sizeof(m_pHisKLineQueryJob->code));
	pQueryParam->hisCounts = QT_STOCK_QUERY_HISKLINE_COUNTS;
	pQueryParam->pHisKLineBuf = m_pHisKLineQueryJob->hisKLine;
	pQueryParam->pStockAgent = this;
	m_pDataTask->PostPktByEvent(pQueryParam);
}

BOOL CQtStockAgent::OnInit()
{
	dllInit(&m_listTraceLog);
	return CQtTimeAgent::OnInit();
}

void CQtStockAgent::OnActive()
{

}

void CQtStockAgent::OnTimeout()
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_updateCmd & QT_STOCK_AGENT_UPDATE_MANAGER_STEP)
	{
		m_updateCmd &=~QT_STOCK_AGENT_UPDATE_MANAGER_STEP;
		emit NotifyUiManagerLoadingProgress();
	}


	if (m_updateCmd & QT_STOCK_AGENT_QUERY_HISKLINE_RESPONESE)
	{
		m_updateCmd  &= ~QT_STOCK_AGENT_QUERY_HISKLINE_RESPONESE;
		emit NotifyUiHisKLineResponese();
	}

	if (!DLL_IS_EMPTY(&m_listTraceLog))
		emit NotifyUiStockTrace();

}