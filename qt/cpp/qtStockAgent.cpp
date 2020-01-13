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
	m_pTraceRecordQueryJob = NULL;
	memset(&m_loadingManager, 0, sizeof(m_loadingManager));
	memset(&m_resetTraceJob, 0, sizeof(m_resetTraceJob));
	memset(&m_realKLineQueryJob, 0, sizeof(m_realKLineQueryJob));
	memset(&m_traceInfoQueryJob, 0, sizeof(m_traceInfoQueryJob));
	memset(&m_sellStatQueryJob, 0, sizeof(m_sellStatQueryJob));
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

	int recordQueyJobSize = sizeof(QT_STOCK_TRACERECORD_QUERY_JOB)+ QT_STOCK_TRACERECORD_QUEY_BUF_COUNT*sizeof(QT_STOCK_TRACERECORD_INFO);
	m_pTraceRecordQueryJob = (QT_STOCK_TRACERECORD_QUERY_JOB*)calloc(1, sizeof(QT_STOCK_TRACERECORD_QUERY_JOB)+ recordQueyJobSize);
	if (m_pTraceRecordQueryJob == NULL)
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

	if (m_pTraceRecordQueryJob != NULL)
	{
		free(m_pTraceRecordQueryJob);
		m_pTraceRecordQueryJob = NULL;
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

int CQtStockAgent::GetAckStockTrace(QT_STOCK_TRACE_LOG* pTraceLog, int counts)
{
	union
	{
		QT_STOCK_TRACE_LOG_NODE* pTraceLogNode;
		DL_NODE*		pNode;
	};

	int leftCounts = counts;
	CSingleLock lock(&m_cs, TRUE);
	while (leftCounts > 0)
	{
		pNode = dllGet(&m_listTraceLog);
		if (pNode == NULL)
			break;

		leftCounts--;
		memcpy(pTraceLog++, &pTraceLogNode->traceLogVal, sizeof(QT_STOCK_TRACE_LOG));
		free(pTraceLogNode);
	}
	return (counts - leftCounts);
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

BOOL CQtStockAgent::GetStockTraceInfo(QT_STOCK_TRACE_INFO* pTraceInfo)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_traceInfoQueryJob.jobResult < 0)
		return FALSE;

	memcpy(pTraceInfo, &m_traceInfoQueryJob.traceInfo, sizeof(QT_STOCK_TRACE_INFO));
	return TRUE;
}

BOOL CQtStockAgent::GetStockSellStat(QT_STOCK_SELLSTAT_INFO_EXE* pSellStat)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_sellStatQueryJob.jobResult < 0)
		return FALSE;

	memcpy(pSellStat->code, m_sellStatQueryJob.sellStatInfo.traceInfo.code, STOCK_CODE_NAME_MAX);
	memcpy(pSellStat->name, m_sellStatQueryJob.sellStatInfo.traceInfo.name, STOCK_CODE_NAME_MAX);
	pSellStat->traceStep = m_sellStatQueryJob.sellStatInfo.traceInfo.traceStep;
	pSellStat->fBuyVal = m_sellStatQueryJob.sellStatInfo.traceInfo.fBuyVal;
	pSellStat->fTopVal = m_sellStatQueryJob.sellStatInfo.traceInfo.fTopVal;

	pSellStat->fCurVal = m_sellStatQueryJob.sellStatInfo.realKline.fClose;
	pSellStat->fMa = m_sellStatQueryJob.sellStatInfo.realKline.fMa10;
	pSellStat->fRsi = m_sellStatQueryJob.sellStatInfo.realKline.fRsi7;
	return TRUE;
}

int	 CQtStockAgent::GetStockTraceRecord(QT_STOCK_TRACERECORD_INFO* pBuf, int offset, int counts)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_pTraceRecordQueryJob->jobResult <= 0)
		return 0;

	int left = m_pTraceRecordQueryJob->jobResult - offset;
	if (left <= 0)
		return 0;

	if (counts > left)
		counts = left;
	memcpy(pBuf, m_pTraceRecordQueryJob->recordBuf+offset, counts*sizeof(QT_STOCK_TRACERECORD_INFO));
	return counts;
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
		QT_STOCK_TRACEINFO_QUERY_PARAM*	pQueryTraceInfo;
		QT_STOCK_TRACERECORD_QUERY_PARAM* pQueryTraceRecord;
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

	case STOCK_QT_EVENT_QUERY_STOCK_TRACEINFO:
		pQueryTraceInfo->pStockAgent->OnTraceInfoQueryResponse(result, pQueryTraceInfo);
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_SELLSTAT_REALKLINE:
		pQueryRealKLine->pStockAgent->OnSellStatQueryRealKLineResponse(result, pQueryRealKLine);
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_SELLSTAT_TRACEINFO:
		pQueryTraceInfo->pStockAgent->OnSellStatQueryTraceInfoResponse(result, pQueryTraceInfo);
		break;

	case STOCK_QT_EVENT_QUERY_TRACE_RECORD:
		pQueryTraceRecord->pStockAgent->OnTraceRecordQueryResponse(result, pQueryTraceRecord);
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

void CQtStockAgent::OnTraceInfoQueryResponse(int result, QT_STOCK_TRACEINFO_QUERY_PARAM* pTraceInfo)
{
	CSingleLock lock(&m_cs, TRUE);
	m_traceInfoQueryJob.jobResult = result;
	m_traceInfoQueryJob.traceInfo.traceStep = pTraceInfo->traceStep;
	m_traceInfoQueryJob.traceInfo.highTime = pTraceInfo->highTime;
	m_traceInfoQueryJob.traceInfo.fHighVal = pTraceInfo->fHighVal;
	m_traceInfoQueryJob.traceInfo.buyTime = pTraceInfo->buyTime;
	m_traceInfoQueryJob.traceInfo.fBuyVal = pTraceInfo->fBuyVal;
	m_traceInfoQueryJob.traceInfo.topTime = pTraceInfo->topTime;
	m_traceInfoQueryJob.traceInfo.fTopVal = pTraceInfo->fTopVal;
	m_traceInfoQueryJob.traceInfo.sellTime = pTraceInfo->sellTime;
	m_traceInfoQueryJob.traceInfo.fSellVal = pTraceInfo->fSellVal;
	m_updateCmd |= QT_STOCK_AGENT_QUERY_TRACEINFO_RESPONESE;
}

void CQtStockAgent::OnSellStatQueryRealKLineResponse(int result, QT_STOCK_REALKLINE_QUERY_PARAM* pRealKLine)
{
	if (result < 0)
	{
	_JOB_ERROR:
		CSingleLock lock(&m_cs, TRUE);
		m_sellStatQueryJob.jobResult = result;
		m_updateCmd |= QT_STOCK_AGENT_QUERY_SELLSTAT_RESPONESE;
		return;
	}

	QT_STOCK_TRACEINFO_QUERY_PARAM* pQueryParam = (QT_STOCK_TRACEINFO_QUERY_PARAM*)m_pManager->AllocPktByEvent(STOCK_QT_EVENT_QUERY_STOCK_SELLSTAT_TRACEINFO, sizeof(QT_STOCK_TRACEINFO_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
		result = -1;
		goto _JOB_ERROR;
	}
	pQueryParam->pStockAgent = this;
	memcpy(pQueryParam->code, m_sellStatQueryJob.sellStatInfo.traceInfo.code, sizeof(pQueryParam->code));
	m_pManager->PostPktByEvent(pQueryParam);
}

void CQtStockAgent::OnSellStatQueryTraceInfoResponse(int result, QT_STOCK_TRACEINFO_QUERY_PARAM* pTraceInfo)
{
	CSingleLock lock(&m_cs, TRUE);
	m_sellStatQueryJob.jobResult = result;
	m_sellStatQueryJob.sellStatInfo.traceInfo.traceStep = pTraceInfo->traceStep;
	m_sellStatQueryJob.sellStatInfo.traceInfo.highTime = pTraceInfo->highTime;
	m_sellStatQueryJob.sellStatInfo.traceInfo.fHighVal = pTraceInfo->fHighVal;
	m_sellStatQueryJob.sellStatInfo.traceInfo.buyTime = pTraceInfo->buyTime;
	m_sellStatQueryJob.sellStatInfo.traceInfo.fBuyVal = pTraceInfo->fBuyVal;
	m_sellStatQueryJob.sellStatInfo.traceInfo.topTime = pTraceInfo->topTime;
	m_sellStatQueryJob.sellStatInfo.traceInfo.fTopVal = pTraceInfo->fTopVal;
	m_sellStatQueryJob.sellStatInfo.traceInfo.sellTime = pTraceInfo->sellTime;
	m_sellStatQueryJob.sellStatInfo.traceInfo.fSellVal = pTraceInfo->fSellVal;
	m_updateCmd |= QT_STOCK_AGENT_QUERY_SELLSTAT_RESPONESE;
}

void CQtStockAgent::OnTraceRecordQueryResponse(int result, QT_STOCK_TRACERECORD_QUERY_PARAM* pTraceRecord)
{
	CSingleLock lock(&m_cs, TRUE);
	m_pTraceRecordQueryJob->jobResult = result;
	m_updateCmd |= QT_STOCK_AGENT_QUERY_TRACERECORD_RESPONESE;
}

void CQtStockAgent::OnGetQueryHisKLine(QString& code)
{
	
	CSingleLock lock(&m_cs, TRUE);
	if (m_pHisKLineQueryJob->bInWorking)
		return;
	else
		m_pHisKLineQueryJob->bInWorking = TRUE;
	
	m_pHisKLineQueryJob->hisCounts = 0;
	m_pHisKLineQueryJob->jobRsult = 0;
	QString2Char(code, m_pHisKLineQueryJob->code);
	if (m_pDataTask == NULL)
		m_pDataTask = ((CStockAutoManager*)m_pManager)->GetTaskData();

	QT_STOCK_HISKLINE_QUERY_PARAM* pQueryParam = (QT_STOCK_HISKLINE_QUERY_PARAM* )m_pDataTask->AllocPktByEvent(STOCK_QT_EVENT_QUERY_STOCK_HISKLINE, sizeof(QT_STOCK_HISKLINE_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
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
	{
		CSingleLock lock(&m_cs, TRUE);
		if (m_realKLineQueryJob.bInWorking)
			return;
		else
			m_realKLineQueryJob.bInWorking = TRUE;
	}

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

void CQtStockAgent::OnGetQueryTraceInfo(QString& code, QString& name)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_traceInfoQueryJob.bInWorking)
		return;
	else
		m_traceInfoQueryJob.bInWorking = TRUE;
	

	m_traceInfoQueryJob.jobResult = 0;
	memset(&m_traceInfoQueryJob.traceInfo, 0, sizeof(m_traceInfoQueryJob.traceInfo));
	QString2Char(code, m_traceInfoQueryJob.traceInfo.code);
	QString2Char(name, m_traceInfoQueryJob.traceInfo.name);

	QT_STOCK_TRACEINFO_QUERY_PARAM* pQueryParam = (QT_STOCK_TRACEINFO_QUERY_PARAM*)m_pManager->AllocPktByEvent(STOCK_QT_EVENT_QUERY_STOCK_TRACEINFO, sizeof(QT_STOCK_TRACEINFO_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
		m_traceInfoQueryJob.jobResult = -1;
		m_updateCmd |= QT_STOCK_AGENT_QUERY_TRACEINFO_RESPONESE;

		return;
	}
	pQueryParam->pStockAgent = this;
	memcpy(pQueryParam->code, m_traceInfoQueryJob.traceInfo.code, sizeof(pQueryParam->code));
	m_pManager->PostPktByEvent(pQueryParam);
}

void CQtStockAgent::OnGetQuerySellStat(QString& code, QString& name)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_sellStatQueryJob.bInWorking)
		return;
	else
		m_sellStatQueryJob.bInWorking = TRUE;
	
	m_sellStatQueryJob.jobResult = 0;
	memset(&m_sellStatQueryJob.sellStatInfo, 0, sizeof(m_sellStatQueryJob.sellStatInfo));
	QString2Char(code, m_sellStatQueryJob.sellStatInfo.traceInfo.code);
	QString2Char(name, m_sellStatQueryJob.sellStatInfo.traceInfo.name);

	if (m_pUpdateTask == NULL)
		m_pUpdateTask = ((CStockAutoManager*)m_pManager)->GetTaskUpdate();

	QT_STOCK_REALKLINE_QUERY_PARAM* pQueryParam = (QT_STOCK_REALKLINE_QUERY_PARAM*)m_pUpdateTask->AllocPktByEvent(STOCK_QT_EVENT_QUERY_STOCK_SELLSTAT_REALKLINE, sizeof(QT_STOCK_REALKLINE_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
		m_sellStatQueryJob.jobResult = -1;
		m_updateCmd |= QT_STOCK_AGENT_QUERY_SELLSTAT_RESPONESE;

		return;
	}

	pQueryParam->pStockAgent = this;
	pQueryParam->pRealKLineBuf = &m_sellStatQueryJob.sellStatInfo.realKline;
	memcpy(pQueryParam->code, m_sellStatQueryJob.sellStatInfo.traceInfo.code, sizeof(pQueryParam->code));
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

void CQtStockAgent::OnGetQueryTraceRecord(int start, int end, int offset)
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_pTraceRecordQueryJob->bInWorking)
		return;
	else
		m_pTraceRecordQueryJob->bInWorking = TRUE;

	if (m_pDataTask == NULL)
		m_pDataTask = ((CStockAutoManager*)m_pManager)->GetTaskData();

	m_pTraceRecordQueryJob->jobResult = 0;
	QT_STOCK_TRACERECORD_QUERY_PARAM* pQueryParam = (QT_STOCK_TRACERECORD_QUERY_PARAM*)m_pDataTask->AllocPktByEvent(STOCK_QT_EVENT_QUERY_TRACE_RECORD, sizeof(QT_STOCK_TRACERECORD_QUERY_PARAM),
		QtTaskEventComplete, NULL);

	if (pQueryParam == NULL)
	{
		m_pTraceRecordQueryJob->jobResult = -1;
		m_updateCmd |= QT_STOCK_AGENT_QUERY_TRACERECORD_RESPONESE;

		return;
	}


	pQueryParam->counts = QT_STOCK_TRACERECORD_QUEY_BUF_COUNT;
	pQueryParam->startTime = start;
	pQueryParam->endTime = end;
	pQueryParam->offset = offset;
	pQueryParam->pStockAgent = this;
	pQueryParam->pTraceRecordInfo = m_pTraceRecordQueryJob->recordBuf;
	m_pDataTask->PostPktByEvent(pQueryParam);
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
		m_pHisKLineQueryJob->bInWorking = FALSE;
		emit NotifyUiHisKLineResponese();
	}

	if (m_updateCmd & QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE;
		m_realKLineQueryJob.bInWorking = FALSE;
		emit NotifyUiRealKLineResponese();
	}

	if (m_updateCmd & QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE;
		emit NotifyResetTraceResponese();
	}

	if (m_updateCmd & QT_STOCK_AGENT_QUERY_TRACEINFO_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_QUERY_TRACEINFO_RESPONESE;
		m_traceInfoQueryJob.bInWorking = FALSE;
		emit NotifyUiTraceInfoResponse();
	}

	if (m_updateCmd & QT_STOCK_AGENT_QUERY_SELLSTAT_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_QUERY_SELLSTAT_RESPONESE;
		m_sellStatQueryJob.bInWorking = FALSE;
		emit NotifyUiSellStatResponse();
	}

	if (m_updateCmd & QT_STOCK_AGENT_QUERY_TRACERECORD_RESPONESE)
	{
		m_updateCmd &= ~QT_STOCK_AGENT_QUERY_TRACERECORD_RESPONESE;
		m_pTraceRecordQueryJob->bInWorking = FALSE;
		emit NotifyUiTraceRecordResponse();
	}


	if (!DLL_IS_EMPTY(&m_listTraceLog))
		emit NotifyUiStockTrace();

}

void CQtStockAgent::OnSignalInActive()
{
	CQtTimeAgent::OnInActive();
}