/*
*qtStockAgent.cpp
*
*/

#include <stockCalcDef.h>
#include <multiEventsTask.h>
#include <stockAutoManager.h>
#include "../include/qtStockTraceDef.h"
#include "../include/qtGlobal.h"
#include "../include/qtStockAgent.h"

/*EXTERN*/
extern CStockConfigTask g_configTask;

CQtStockAgent::CQtStockAgent(QObject* parent): CQtTimeAgent(parent)
{
	m_updateCmd = 0;
	m_pManager = NULL;
	m_pDataTask = NULL;
	m_pUpdateTask = NULL;
	memset(&m_loadingManager, 0, sizeof(m_loadingManager));
	memset(&m_resetTraceJob, 0, sizeof(m_resetTraceJob));
	dllInit(&m_listTraceLog);
}

CQtStockAgent::~CQtStockAgent()
{
	Close();
}

BOOL CQtStockAgent::Create(QThread* pServerObj, CMultiEventsTask* pManager, int timeoutMs)
{
	m_pHisKLineQueryJob = (QT_STOCK_HISKLINE_QUERY_JOB*)calloc(1, sizeof(QT_STOCK_HISKLINE_QUERY_JOB));
	if (m_pHisKLineQueryJob == NULL)
		return FALSE;

	m_pManager = pManager;
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

BOOL CQtStockAgent::UpdateStockTrace(char const* pStockName,  char const* pStockCode, UINT updateStat)
{
	QT_STOCK_TRACE_LOG_NODE* pTraceLogNode = (QT_STOCK_TRACE_LOG_NODE*)calloc(sizeof(QT_STOCK_TRACE_LOG_NODE), 1);
	if (pTraceLogNode == NULL)
		return FALSE;

	pTraceLogNode->traceLogVal.stat = updateStat;
	strncpy(pTraceLogNode->traceLogVal.stockName, pStockName, STOCK_CODE_NAME_MAX);
	strncpy(pTraceLogNode->traceLogVal.code, pStockCode, sizeof(pTraceLogNode->traceLogVal.code)-1);

	CSingleLock lock(&m_cs, TRUE);
	m_updateCmd |= QT_STOCK_AGENT_UPDATE_STOCK_TRACE;
	dllAdd(&m_listTraceLog,&pTraceLogNode->node);

	return TRUE;
}


void CQtStockAgent::GetAutoManagerLoading(QT_STOCK_LOADING_MANAGER* pLoadingInfo)
{
	CSingleLock lock(&m_cs, TRUE);
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

	memcpy(pKLine, m_pHisKLineQueryJob->hisKLine+offset, sizeof(STOCK_CALC_TRACE_KLINE)*left);

	return left;
}

BOOL CQtStockAgent::GetStockRealKLine(QT_STOCK_REALKLINE_INFO* pKLineInfo)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_realKLineQueryJob.jobResult < 0)
		return FALSE;

	memcpy(pKLineInfo, &m_realKLineQueryJob.realKLine, sizeof(QT_STOCK_REALKLINE_INFO));
	return TRUE;
}

void CQtStockAgent::GetConfigPython(STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	g_configTask.GetConfigPython(pConfigPython);
}

void CQtStockAgent::GetConfigData(STOCKAUTO_CONFIG_DATA* pConfigData)
{
	g_configTask.GetConfigData(pConfigData);
}

void CQtStockAgent::GetConfigTrace(STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	g_configTask.GetConfigTrace(pConfigTrace);
}

void CQtStockAgent::UpdateConfigPython(STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	g_configTask.UpdateConfigPython(pConfigPython);
}

void CQtStockAgent::UpdateConfigData(STOCKAUTO_CONFIG_DATA* pConfigData)
{
	g_configTask.UpdateConfigData(pConfigData);
}

void CQtStockAgent::UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	g_configTask.UpdateConfigTrace(pConfigTrace);
}

void CQtStockAgent::SyncConfig()
{
	g_configTask.SyncConfig();
}

void CQtStockAgent::UpdateResetTraceResult(int result)
{
	CSingleLock lock(&m_cs, TRUE);
	m_resetTraceJob.jobResult = result;
	m_updateCmd |= QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE;
}

BOOL CQtStockAgent::QtTaskEventComplete(UINT cmd, int result, void* param, int paramLen)
{
	union
	{
		QT_STOCK_HISKLINE_QUERY_PARAM* pQueryHisKLine;
		QT_STOCK_REALKLINE_QUERY_PARAM* pQueryRealKLine;
	};

	pQueryHisKLine = (QT_STOCK_HISKLINE_QUERY_PARAM*)param;

	switch (cmd)
	{
	default:
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_HISKLINE:
		pQueryHisKLine->pStockAgent->OnHisKLineQueryResponse(result, pQueryHisKLine);
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_REALKLINE:
		pQueryRealKLine->pStockAgent->OnRealKLineQueryResponse(result, pQueryRealKLine);
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


void CQtStockAgent::OnRealKLineQueryResponse(int result, QT_STOCK_REALKLINE_QUERY_PARAM* pRealKLine)
{
	CSingleLock lock(&m_cs, TRUE);
	m_realKLineQueryJob.jobResult = result;
	m_updateCmd |= QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE;
}

void CQtStockAgent::OnGetQueryHisKLine(QString& code)
{
	m_pHisKLineQueryJob->hisCounts = 0;
	m_pHisKLineQueryJob->jobRsult = 0;
	QString2Char(code, m_pHisKLineQueryJob->code);
	if (m_pDataTask == NULL)
		m_pDataTask = ((CStockAutoManager*)m_pManager)->GetTaskData();

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

void CQtStockAgent::OnGetQueryRealKLine(QString& code, QString& name)
{
	m_realKLineQueryJob.jobResult = 0;
	memset(&m_realKLineQueryJob.realKLine, 0, sizeof(m_realKLineQueryJob.realKLine));
	QString2Char(code, m_realKLineQueryJob.realKLine.code);
	QString2Char(name, m_realKLineQueryJob.realKLine.name);
	if (m_pUpdateTask == NULL)
		m_pUpdateTask = ((CStockAutoManager*)m_pManager)->GetTaskUpdate();


	QT_STOCK_REALKLINE_QUERY_PARAM* pQueryParam = (QT_STOCK_REALKLINE_QUERY_PARAM*)m_pUpdateTask->AllocPktByEvent(STOCK_QT_EVENT_QUERY_STOCK_REALKLINE, sizeof(QT_STOCK_REALKLINE_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
		CSingleLock lock(&m_cs, TRUE);
		m_realKLineQueryJob.jobResult = -1;
		m_updateCmd |= QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE;

		return;
	}

	memcpy(pQueryParam->code, m_realKLineQueryJob.realKLine.code, sizeof(pQueryParam->code));
	pQueryParam->pRealKLineBuf = &m_realKLineQueryJob.realKLine.realKLine;
	pQueryParam->pStockAgent = this;
	m_pUpdateTask->PostPktByEvent(pQueryParam);
}

void CQtStockAgent::OnRequestResetTrace()
{
	STOCK_CALC_RESET_TRACE_CALC* pResetTrace = (STOCK_CALC_RESET_TRACE_CALC*)m_pManager->AllocPktByEvent(STOCK_CALC_EVENT_RESET_TRACE_CALC, 
		sizeof(STOCK_CALC_RESET_TRACE_CALC), NULL, NULL);
	if (pResetTrace == NULL)
	{
		CSingleLock lock(&m_cs, TRUE);
		m_resetTraceJob.jobResult = -1;
		m_updateCmd |= QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE;

		return;
	}

	m_pManager->PostPktByEvent(pResetTrace);
}

BOOL CQtStockAgent::OnInit()
{
	dllInit(&m_listTraceLog);
	return CQtTimeAgent::OnInit();
}

void CQtStockAgent::OnActive()
{
	CQtTimeAgent::OnActive();
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

	if (m_updateCmd & QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE;
		emit NotifyUiRealKLineResponese();
	}

	if (m_updateCmd & QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE;
		emit NotifyResetTraceResponese();
	}

	if (!DLL_IS_EMPTY(&m_listTraceLog))
		emit NotifyUiStockTrace();

}

void CQtStockAgent::OnSignalInActive()
{
	CQtTimeAgent::OnInActive();
}