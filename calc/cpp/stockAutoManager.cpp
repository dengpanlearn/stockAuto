/*
*stockAutoManager.cpp
*/
#include <qdatetime.h>
#include "../include/stockConfigTask.h"
#include <stockAutoWindow.h>
#include "../include/stockTraceBase.h"
#include "../include/stockTraceReal.h"
#include "../include/stockTraceWeek.h"
#include "../include/stockAutoManager.h"

/*EXTERN*/
extern CStockConfigTask g_configTask;

CStockAutoManager::CStockAutoManager()
{
	m_managerStep = STOCK_AUTO_MANAGER_STEP_NONE;
	m_pUpdateTask = NULL;
	m_pJobList = NULL;
	m_pDataTask = NULL;
	m_pJobTraceLog = NULL;
	m_pTraceWeek = NULL;
	m_pTraceReal = NULL;
	m_eventTraceMan = INVALID_DP_EVENT_ID;
	memset(&m_jobListUpdate, 0, sizeof(m_jobListUpdate));
	memset(&m_jobHisKLineUpdate, 0, sizeof(m_jobHisKLineUpdate));
	memset(&m_jobResetTrace, 0, sizeof(m_jobResetTrace));
	dllInit(&m_listTraceWeek);
	dllInit(&m_listTraceReal);
}

CStockAutoManager::~CStockAutoManager()
{
	Close();
}

BOOL CStockAutoManager::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize, CStockAutoWindow* pAutoWindow)
{
	int jobListSize = sizeof(STOCK_MANAGER_JOB_LIST) + STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_CODE_NAME);
	int jobTraceLogSize = sizeof(STOCK_MANAGER_JOB_TRACELOG_LOAD) + 2*STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_MANAGER_TRACE_LOG);

	TCHAR traceManEventName[32] = { 0 };

#ifdef UNICODE
	swprintf_s(traceManEventName, countof(traceManEventName) - 1, _T("%s%s"), TRACE_MANAGER_TASK_EVENT_NAME_PREFIX, pNameTask);
#else
	sprintf_s(traceManEventName, countof(traceManEventName) - 1, _T("%s%s"), TRACE_MANAGER_TASK_EVENT_NAME_PREFIX, pNameTask);
#endif 

	do
	{
		m_eventTraceMan = dpEventCreate(FALSE, traceManEventName, FALSE);
		if (m_eventTraceMan == INVALID_DP_EVENT_ID)
			break;

		m_pDataTask = new CStockDataTask();
		if (m_pDataTask == NULL)
			break;

		if (!m_pDataTask->Create(STOCK_DATA_TASK_NAME, STOCK_DATA_TASK_SZ, STOCK_DATA_TASK_PRI, 0,
			STOCK_DATA_TASK_TIMEOUT, STOCK_DATA_TASK_EVENTS, STOCK_UPDATE_TASK_EVENT_PARAM_SIZE))
			break;

		m_pJobList = (STOCK_MANAGER_JOB_LIST*)calloc(1, jobListSize);
		if (m_pJobList == NULL)
			break;

		m_pJobTraceLog = (STOCK_MANAGER_JOB_TRACELOG_LOAD*)calloc(1, jobTraceLogSize);
		if (m_pJobTraceLog == NULL)
			break;

		m_pCalcTraceBuf = (STOCK_CALC_TRACE_NODE*)calloc(STOCK_AUTO_COUNTS_MAX, sizeof(STOCK_CALC_TRACE_NODE));
		if (m_pCalcTraceBuf == NULL)
			break;

		m_pJobList->stockCounts = STOCK_AUTO_COUNTS_MAX;
		m_pUpdateTask = new CStockUpdateTask();
		if (m_pUpdateTask == NULL)
			break;

		if (!m_pUpdateTask->Create(STOCK_UPDATE_TASK_NAME, STOCK_UPDATE_TASK_SZ, STOCK_UPDATE_TASK_PRI, 0,
			STOCK_UPDATE_TASK_TIMEOUT, STOCK_UPDATE_TASK_EVENTS, STOCK_UPDATE_TASK_EVENT_PARAM_SIZE))
			break;
	
		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents, maxEventParamSize))
			break;
		
		InitConfig();
		m_managerStep = STOCK_AUTO_MANAGER_STEP_LIST_INIT;
		m_pAutoWindow = pAutoWindow;
		ActiveManager();
		return TRUE;
	} while (FALSE);

	Close();
	return FALSE;

}

void CStockAutoManager::Close()
{
	CMultiEventsTask::Close();

	if (m_pTraceWeek != NULL)
	{
		delete m_pTraceWeek;
		m_pTraceWeek = NULL;
	}

	if (m_pTraceReal != NULL)
	{
		delete m_pTraceReal;
		m_pTraceReal = NULL;
	}
	if (m_pUpdateTask != NULL)
	{
		delete m_pUpdateTask;
		m_pUpdateTask = NULL;
	}

	if (m_pCalcTraceBuf != NULL)
	{
		free(m_pCalcTraceBuf);
		m_pCalcTraceBuf = NULL;
	}
	if (m_pJobTraceLog != NULL)
	{
		free(m_pJobTraceLog);
		m_pJobTraceLog = NULL;
	}
	if (m_pJobList != NULL)
	{
		free(m_pJobList);
		m_pJobList = NULL;
	}

	if (m_pDataTask != NULL)
	{
		delete m_pDataTask;
		m_pDataTask = NULL;
	}

	if (m_eventTraceMan != INVALID_DP_EVENT_ID)
	{
		dpEventClose(m_eventTraceMan);
		m_eventTraceMan = INVALID_DP_EVENT_ID;
	}
}

void CStockAutoManager::InitConfig()
{
	g_configTask.GetConfigTrace(&m_traceConfig);
#if 0
	m_traceConfig.raiseBalances = STOCKAUTO_CONFIG_TRACE_RAISE_BALANCES_DFT;
	m_traceConfig.fRaisePercent = STOCKAUTO_CONFIG_TRACE_RAISE_PERCENT_DFT;
	m_traceConfig.rsiBuyWaits = STOCKAUTO_CONFIG_TRACE_RSI_BUY_WAIT_DFT;
	m_traceConfig.fRsiBuy = STOCKAUTO_CONFIG_TRACE_RSI_BUY_DFT;
	m_traceConfig.reachHighRanges = STOCKAUTO_CONFIG_TRACE_REAHHIGH_RANGES_DFT;
	m_traceConfig.rsiSellWaits = STOCKAUTO_CONFIG_TRACE_RSI_SELL_WAITS_DFT;
	m_traceConfig.fRsiSell = STOCKAUTO_CONFIG_TRACE_RSI_SELL_DFT;
	m_traceConfig.fCutLossPercent = STOCKAUTO_CONFIG_TRACE_CUTLOSS_PERCENT_DFT;
#endif
}

void CStockAutoManager::OnEventManager()
{
	switch (m_managerStep)
	{
	default:
	case STOCK_AUTO_MANAGER_STEP_NONE:
		m_managerStep = STOCK_AUTO_MANAGER_STEP_LIST_INIT;
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_INIT:
		m_managerStep = OnStockAutoManagerInit();
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_UPDATING:
		m_managerStep = OnStockAutoManagerListUpdate();
		break;

	case STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING:
		m_managerStep = OnStockAutoManagerHisKLineUpdate();
		break;

	case STOCK_AUTO_MANAGER_STEP_ERROR:
		break;

	case STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING:
		m_managerStep = OnStockAutoManagerTraceLogLoad();
		break;

	case STOCK_AUTO_MANAGER_STEP_STOCK_TRACING:
		m_managerStep = OnStockAutoManagerTrace();
		break;

	case STOCK_AUTO_MANAGER_STEP_RESET_TRACE:
		m_managerStep = OnStockAutoManagerResetTrace();
		break;
	}
}

void CStockAutoManager::ActiveManager()
{
	dpEventSet(m_eventTraceMan);
}

void CStockAutoManager::InactiveManager()
{
	dpEventReset(m_eventTraceMan);
}

BOOL CStockAutoManager::UpdateStockTraceStat(int stockIdx, char const* pStockCode, UINT updateStat)
{
	return m_pAutoWindow->UpdateStockTrace(m_pJobList->codeName[stockIdx].name, pStockCode, updateStat);
}

const char* CStockAutoManager::GetStockNameByCode(const char* pCode)
{
	for (int i = 0; i < m_pJobList->stockCounts; i++)
	{
		if (strcmp(pCode, m_pJobList->codeName[i].code) == 0)
			return m_pJobList->codeName[i].name;
	}

	return NULL;
}

BOOL CStockAutoManager::UpdateCalcTraceConfig(STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	STOCK_CALC_UPDATE_CONFIG_TRACE* pPkt = (STOCK_CALC_UPDATE_CONFIG_TRACE* )AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_CONFIG_TRACE, sizeof(STOCK_CALC_UPDATE_CONFIG_TRACE), NULL, NULL);
	if (pPkt == NULL)
		return FALSE;

	memcpy(&pPkt->configTrace, pConfigTrace, sizeof(STOCKAUTO_CONFIG_TRACE));
	PostPktByEvent(pPkt);
	return TRUE;
}

int CStockAutoManager::GetTaskEvent(DP_EVENT_ID* pEventsBuf, int maxCount)const
{
	// 基类的event 在前
	int eventCount = CMultiEventsTask::GetTaskEvent(pEventsBuf, maxCount);
	if (eventCount > 0)
	{
		maxCount -= eventCount;

		if (maxCount < 1)
			return -1;

		pEventsBuf[eventCount++] = m_eventTraceMan;
	}

	return eventCount;
}

int CStockAutoManager::GetTaskEventCount()const
{
	return CMultiEventsTask::GetTaskEventCount() + 1;
}


void CStockAutoManager::OnSubEventActive(UINT evnetNum)
{
	if (evnetNum == (GetTaskEventCount() - 1))
	{
		InactiveManager();
		OnEventManager();
	}
}

void CStockAutoManager::OnExit()
{
	m_pUpdateTask->StopWork();
	m_pUpdateTask->WaitWorkExit(2000);
	m_pDataTask->StopWork();
	m_pDataTask->WaitWorkExit(2000);
}

UINT CStockAutoManager::PreActive(UINT timeout)
{
	if (m_managerStep == STOCK_AUTO_MANAGER_STEP_STOCK_TRACING)
	{
		timeout = STOCK_AUTO_MANAGER_TASK_TIMEOUT_TRACE;
	}
	
	return CMultiEventsTask::PreActive(timeout);
}

BOOL CStockAutoManager::CheckSelf()
{
	return TRUE;
}

int CStockAutoManager::OnEventActive(UINT cmd, void* param, int paramLen)
{
	int result = EVENT_COMPLETE_OK;
	union
	{
		STOCK_CALC_GET_LIST_RESP* pGetListResp;
		STOCK_CALC_UPDATE_LIST_RESP* pUpdateListResp;
		STOCK_CALC_LOAD_TRACELOG_RESP* pLoadTraceLogResp;
		STOCK_CALC_UPDATE_HISKLINE_RESP* pUpdateHisKLineResp;
		STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp;
		STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp;
		STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp;
		STOCK_CALC_UPDATE_CONFIG_TRACE*	pUpdateConfigTrace;
		STOCK_CALC_RESET_TRACE_CALC*	pResetTraceCalc;
		STOCK_CALC_CLEAR_TRACE_HISTIME_RESP*	pClearHisTimeResp;
		QT_STOCK_TRACEINFO_QUERY_PARAM* pQueryTraceInfo;
		STOCK_CALC_INSERT_TRACERECORD_RESP* pInsertTraceRecordResp;
	};

	pGetListResp = (STOCK_CALC_GET_LIST_RESP*)param;

	switch (cmd)
	{
	case STOCK_CALC_EVENT_GET_STOCK_LIST_RESP:	
		OnStockListGetResp(pGetListResp);
		break;

	case STOCK_CALC_EVENT_UPDATE_STOCK_LIST_RESP:
		OnStockListUpdateResp(pUpdateListResp);
		break;

	case STOCK_CALC_EVENT_LOAD_TRACE_RESP:
		OnTraceLogLoadResp(pLoadTraceLogResp);
		break;

	case STOCK_CALC_EVENT_UPDATE_STOCK_HISKLINE_RESP:
		OnHisKLineUpdateResp(pUpdateHisKLineResp);
		break;

	case STOCK_CALC_EVENT_GET_STOCK_HISKLINE_RESP:
		OnHisKLineGetResp(pGetHisKLineResp);
		break;

	case STOCK_CALC_EVENT_UPDATE_TRACE_LOG_RESP:
		OnUpdateTraceLogResp(pUpdateTraceLogResp);
		break;

	case STOCK_CALC_EVENT_GET_CUR_STOCK_HISKLINE_RESP:
		OnCurHisKLineGetResp(pGetCurHisKLineResp);
		break;

	case STOCK_CALC_EVENT_UPDATE_CONFIG_TRACE:
		OnUpdateConfigTrace(pUpdateConfigTrace);
		break;

	case STOCK_CALC_EVENT_RESET_TRACE_CALC:
		OnResetTraceCalc(pResetTraceCalc);
		break;

	case STOCK_CALC_EVENT_CLEAR_TRACE_HISTIME_RESP:
		OnClearTraceHistimeResp(pClearHisTimeResp);
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_TRACEINFO:
	case STOCK_QT_EVENT_QUERY_STOCK_SELLSTAT_TRACEINFO:
		result = OnQueryTraceInfo(pQueryTraceInfo);
		break;

	case STOCK_CALC_EVENT_INSERT_TRACE_RECORD_RESP:
		OnInsertTraceRecordResp(pInsertTraceRecordResp);
		break;
	}

	return result;
}

BOOL CStockAutoManager::OnEventComplete(UINT cmd, int result, void* param, int paramLen)
{
	BOOL re = TRUE;
	switch (cmd)
	{
	case STOCK_CALC_EVENT_GET_STOCK_LIST:
		re = OnStockListGetComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_UPDATE_STOCK_LIST:
		re = OnStockListUpdateComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_LOAD_TRACE_LOG:
		re = OnTraceLogLoadComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_UPDATE_STOCK_HISKLINE:
		re = OnHisKLineUpdateComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_GET_STOCK_HISKLINE:
		re = OnHisKLineGetComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_UPDATE_TRACE_LOG:
		re = OnUpdateTraceLogComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_GET_CUR_STOCK_HISKLINE:
		re = OnCurHisKLineGetComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_CLEAR_TRACE_HISTIME:
		re = OnClearTraceHistimeComplete(result, param, paramLen);
		break;

	case STOCK_CALC_EVENT_INSERT_TRACE_RECORD:
		re = OnInsertTraceRecordComplete(result, param, paramLen);
		break;
	}

	return re;
}

void CStockAutoManager::OnTimeout()
{
	if ((m_pTraceReal != NULL) && m_managerStep != STOCK_AUTO_MANAGER_STEP_RESET_TRACE)
	{
		m_pTraceReal->Trace();
	}

	return CMultiEventsTask::OnTimeout();
}

void CStockAutoManager::InitStockTraceByLog(STOCK_MANAGER_JOB_TRACELOG_LOAD* pJobTraceLog, STOCK_MANAGER_JOB_LIST* pJobList)
{
	int listSize = pJobList->stockCounts;
	STOCK_CODE_NAME* pCodeList = pJobList->codeName;

	int	logCounts = pJobTraceLog->logCounts;
	STOCK_MANAGER_TRACE_LOG* pNewTraceLog = pJobTraceLog->traceLog + STOCK_AUTO_COUNTS_MAX;
	memset(pNewTraceLog, 0, sizeof(STOCK_MANAGER_TRACE_LOG)* STOCK_AUTO_COUNTS_MAX);

	for (int i = 0; i < listSize; i++, pCodeList++, pNewTraceLog++)
	{
		STOCK_MANAGER_TRACE_LOG* pTraceLog = pJobTraceLog->traceLog;
		int j = 0;
		for (; j < logCounts; j++, pTraceLog++)
		{
			if (strcmp(pTraceLog->code, pCodeList->code) == 0)
			{
				memcpy(pNewTraceLog, pTraceLog, sizeof(STOCK_MANAGER_TRACE_LOG));
				break;
			}
		}

		if (j >= logCounts)
		{
			memcpy(pNewTraceLog->code, pCodeList->code, sizeof(pCodeList->code));
		}
	}
}

void CStockAutoManager::InitStockTraceCalc(STOCK_MANAGER_TRACE_LOG* pTraceLog, int logCounts)
{
	union
	{
		DL_NODE* pNode;
		STOCK_CALC_TRACE_NODE*	pTraceNode;
	};

	pTraceNode = m_pCalcTraceBuf;
	for (int j = 0; j < logCounts; j++, pTraceLog++, pTraceNode++)
	{
		pTraceNode->stockIdx = j;
		pTraceNode->pTraceLog = pTraceLog;

		if (pTraceLog->traceStep < CALC_STOCK_TRADE_STEP_WAIT_BUY)
			dllAdd(&m_listTraceWeek, pNode);
		else
		{
			QDateTime realDateTime = QDateTime::fromTime_t(pTraceLog->realTime);
			QDate realDate = realDateTime.date();
			QDate curDate = QDate::currentDate();
			QDate lastWeekEndDate = curDate.addDays(-curDate.dayOfWeek());
			if (realDate <= lastWeekEndDate)
				dllAdd(&m_listTraceWeek, pNode);
			else
				dllAdd(&m_listTraceReal, pNode);
		}		
	}

	if (m_pTraceWeek == NULL)
	{
		CStockTraceWeek* pTraceWeek = new CStockTraceWeek(this, &m_listTraceWeek);
		if (pTraceWeek != NULL)
		{
			if (!pTraceWeek->Init(STOCK_HIS_KLINE_MAX_COUNTS, &m_traceConfig))
			{
				delete pTraceWeek;
				pTraceWeek = NULL;
			}
		}

		m_pTraceWeek = pTraceWeek;
	}

	if (m_pTraceReal == NULL)
	{
		CStockTraceReal* pTraceReal = new CStockTraceReal(this, &m_listTraceReal);
		if (pTraceReal != NULL)
		{
			if (!pTraceReal->Init(STOCK_HIS_KLINE_MAX_COUNTS, &m_traceConfig))
			{
				delete pTraceReal;
				pTraceReal = NULL;
			}
		}

		m_pTraceReal = pTraceReal;
	}
}

STOCK_CALC_GET_HISKLINE* CStockAutoManager::AllocGetHisKLinePkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_GET_HISKLINE* pGetHisKLine = (STOCK_CALC_GET_HISKLINE* )m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_GET_STOCK_HISKLINE, sizeof(STOCK_CALC_GET_HISKLINE),
		MultiTaskEventComplete, this);
	if (pGetHisKLine == NULL)
		return NULL;

	pGetHisKLine->pTraceBase = pTraceBase;
	return pGetHisKLine;
}

void CStockAutoManager::PostGetHisKLinePkt(STOCK_CALC_GET_HISKLINE* pGetHisKLine)
{
	m_pDataTask->PostPktByEvent(pGetHisKLine);
}

STOCK_CALC_GET_HISKLINE_RESP* CStockAutoManager::AllocGetHisKLineRespPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp = (STOCK_CALC_GET_HISKLINE_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_GET_STOCK_HISKLINE_RESP, sizeof(STOCK_CALC_GET_HISKLINE_RESP),
		NULL, this);
	if (pGetHisKLineResp == NULL)
		return NULL;

	pGetHisKLineResp->pTraceBase = pTraceBase;
	return pGetHisKLineResp;
}

void CStockAutoManager::PostGetHisKLineRespPkt(STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp)
{
	PostPktByEvent(pGetHisKLineResp);
}

STOCK_CALC_GET_CUR_HISKLINE* CStockAutoManager::AllocGetCurHisKLinePkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_GET_CUR_HISKLINE* pGetCurHisKLine = (STOCK_CALC_GET_CUR_HISKLINE*)m_pUpdateTask->AllocPktByEvent(STOCK_CALC_EVENT_GET_CUR_STOCK_HISKLINE, sizeof(STOCK_CALC_GET_CUR_HISKLINE),
		MultiTaskEventComplete, this);
	if (pGetCurHisKLine == NULL)
		return NULL;

	pGetCurHisKLine->pTraceBase = pTraceBase;
	return pGetCurHisKLine;
}

void CStockAutoManager::PostGetCurHisKLinePkt(STOCK_CALC_GET_CUR_HISKLINE* pGetCurHisKLine)
{
	m_pUpdateTask->PostPktByEvent(pGetCurHisKLine);
}

STOCK_CALC_GET_CUR_HISKLINE_RESP* CStockAutoManager::AllocGetCurHisKLineRespPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp = (STOCK_CALC_GET_CUR_HISKLINE_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_GET_CUR_STOCK_HISKLINE_RESP, sizeof(STOCK_CALC_GET_CUR_HISKLINE_RESP),
		NULL, this);
	if (pGetCurHisKLineResp == NULL)
		return NULL;

	pGetCurHisKLineResp->pTraceBase = pTraceBase;
	return pGetCurHisKLineResp;
}

void CStockAutoManager::PostGetCurHisKLineRespPkt(STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp)
{
	PostPktByEvent(pGetCurHisKLineResp);
}

STOCK_CALC_UPDATE_TRACELOG* CStockAutoManager::AllocUpdateTraceLogPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = (STOCK_CALC_UPDATE_TRACELOG*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_TRACE_LOG, sizeof(STOCK_CALC_UPDATE_TRACELOG),
		MultiTaskEventComplete, this);
	if (pUpdateTraceLog == NULL)
		return NULL;

	pUpdateTraceLog->pTraceBase = pTraceBase;
	return pUpdateTraceLog;
}

void CStockAutoManager::PostUpdateTraceLogPkt(STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog)
{
	m_pDataTask->PostPktByEvent(pUpdateTraceLog);
}

STOCK_CALC_UPDATE_TRACELOG_RESP* CStockAutoManager::AllocUpdateTraceLogRespPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp = (STOCK_CALC_UPDATE_TRACELOG_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_TRACE_LOG_RESP, sizeof(STOCK_CALC_UPDATE_TRACELOG_RESP),
		NULL, this);
	if (pUpdateTraceLogResp == NULL)
		return NULL;

	pUpdateTraceLogResp->pTraceBase = pTraceBase;
	return pUpdateTraceLogResp;
}

void CStockAutoManager::PostUpdateTraceLogRespPkt(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp)
{
	PostPktByEvent(pUpdateTraceLogResp);
}

STOCK_CALC_INSERT_TRACERECORD* CStockAutoManager::AllocInsertTraceRecordPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_INSERT_TRACERECORD* pInsertTraceRecord = (STOCK_CALC_INSERT_TRACERECORD*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_INSERT_TRACE_RECORD, sizeof(STOCK_CALC_INSERT_TRACERECORD),
		MultiTaskEventComplete, this);
	if (pInsertTraceRecord == NULL)
		return NULL;

	pInsertTraceRecord->pTraceBase = pTraceBase;
	return pInsertTraceRecord;
}

void CStockAutoManager::PostInsertTraceRecordPkt(STOCK_CALC_INSERT_TRACERECORD* pInsertTraceRecord)
{
	m_pDataTask->PostPktByEvent(pInsertTraceRecord);
}

STOCK_CALC_INSERT_TRACERECORD_RESP* CStockAutoManager::AllocInsertTraceRecordRespPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_INSERT_TRACERECORD_RESP* pInsertTraceRecordResp = (STOCK_CALC_INSERT_TRACERECORD_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_INSERT_TRACE_RECORD_RESP, sizeof(STOCK_CALC_INSERT_TRACERECORD_RESP),
		NULL, this);
	if (pInsertTraceRecordResp == NULL)
		return NULL;

	pInsertTraceRecordResp->pTraceBase = pTraceBase;
	return pInsertTraceRecordResp;
}

void CStockAutoManager::PostInsertTraceLRecordRespPkt(STOCK_CALC_INSERT_TRACERECORD_RESP* pInsertTraceRecordResp)
{
	PostPktByEvent(pInsertTraceRecordResp);
}

void CStockAutoManager::AddTraceList(CStockTraceBase* pTraceBase, STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (pTraceBase == m_pTraceWeek)
	{
		m_pTraceReal->AddTraceStock(pTraceNode);
	}
	else
	{
		m_pTraceWeek->AddTraceStock(pTraceNode);
	}
}

UINT CStockAutoManager::OnStockAutoManagerInit()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_LIST_INIT;
	if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// 查询
		STOCK_CALC_GET_LIST* pGetListParam = (STOCK_CALC_GET_LIST* )m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_GET_STOCK_LIST, sizeof(STOCK_CALC_GET_LIST),
			MultiTaskEventComplete, this);
	
		if (pGetListParam != NULL)
		{
			pGetListParam->bufCounts = STOCK_AUTO_COUNTS_MAX;
			pGetListParam->pListBuf = m_pJobList->codeName;

			m_pDataTask->PostPktByEvent(pGetListParam);
			m_pJobList->jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING;
	}
	else
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_UPDATING;
	}

	if (m_pJobList->jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		ActiveManager();

	m_pAutoWindow->UpdateAutoManagerStep(nextStep, 0);
	return nextStep;
}

void CStockAutoManager::OnStockListGetResp(STOCK_CALC_GET_LIST_RESP* pGetListResp)
{
	if (m_pJobList->jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pGetListResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_pJobList->stockCounts = pGetListResp->respResult;
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
	ActiveManager();
}

BOOL CStockAutoManager::OnStockListGetComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_GET_LIST_RESP* pGetListResp = (STOCK_CALC_GET_LIST_RESP* )AllocPktByEvent(STOCK_CALC_EVENT_GET_STOCK_LIST_RESP,
		sizeof(STOCK_CALC_GET_LIST_RESP), NULL, this);

	if (pGetListResp == NULL)
		return FALSE;

	pGetListResp->respResult = result;
	PostPktByEvent(pGetListResp);
	return TRUE;
}


UINT CStockAutoManager::OnStockAutoManagerListUpdate()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_LIST_UPDATING;
	if (m_jobListUpdate.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_jobListUpdate.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// 查询
		STOCK_CALC_UPDATE_LIST* pUpdateListParam = (STOCK_CALC_UPDATE_LIST*)m_pUpdateTask->AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_STOCK_LIST,
			sizeof(STOCK_CALC_UPDATE_LIST), MultiTaskEventComplete, this);
		if (pUpdateListParam != NULL)
		{
			m_pUpdateTask->PostPktByEvent(pUpdateListParam);
			m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else if (m_jobListUpdate.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_INIT;
	}
	else
	{
		m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_ERROR;
	}

	if (m_jobListUpdate.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		ActiveManager();

	m_pAutoWindow->UpdateAutoManagerStep(nextStep, 0);
	return nextStep;
}


void CStockAutoManager::OnStockListUpdateResp(STOCK_CALC_UPDATE_LIST_RESP* pUpdateListResp)
{
	if (m_jobListUpdate.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pUpdateListResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
	ActiveManager();
}

BOOL CStockAutoManager::OnStockListUpdateComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_UPDATE_LIST_RESP* pUpdateListResp = (STOCK_CALC_UPDATE_LIST_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_STOCK_LIST_RESP,
		sizeof(STOCK_CALC_UPDATE_LIST_RESP), NULL, this);

	if (pUpdateListResp == NULL)
		return FALSE;

	pUpdateListResp->respResult = result;
	PostPktByEvent(pUpdateListResp);
	return TRUE;
}

UINT CStockAutoManager::OnStockAutoManagerTraceLogLoad()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING;
	if (m_pJobTraceLog->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_pJobTraceLog->jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// 查询
		STOCK_CALC_LOAD_TRACELOG* pTraceLogLoad = (STOCK_CALC_LOAD_TRACELOG*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_LOAD_TRACE_LOG,
			sizeof(pTraceLogLoad), MultiTaskEventComplete, this);
		if (pTraceLogLoad != NULL)
		{
			pTraceLogLoad->bufCounts = STOCK_AUTO_COUNTS_MAX;
			pTraceLogLoad->pLogBuf = m_pJobTraceLog->traceLog;

			m_pDataTask->PostPktByEvent(pTraceLogLoad);
			m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else // 获取成功 失败
	{
		m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_NONE;
		InitStockTraceByLog(m_pJobTraceLog, m_pJobList);

		QDateTime curDateTime = QDateTime::currentDateTime();
		QDate curDate = curDateTime.date();
		int curOffset = curDate.dayOfWeek();
		QDate lastWeekDate = curDate.addDays(-curOffset);
	
		m_jobHisKLineUpdate.lastWeekEndTime = QDateTime(lastWeekDate).toTime_t();
		nextStep = STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING;
	}

	if (m_pJobTraceLog->jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		ActiveManager();

	m_pAutoWindow->UpdateAutoManagerStep(nextStep, 0);
	return nextStep;
}

void CStockAutoManager::OnTraceLogLoadResp(STOCK_CALC_LOAD_TRACELOG_RESP* pTraceLogResp)
{
	if (m_pJobTraceLog->jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pTraceLogResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_pJobTraceLog->logCounts = 0;
		m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_pJobTraceLog->logCounts = pTraceLogResp->respResult;
		m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}

	ActiveManager();
}

BOOL CStockAutoManager::OnTraceLogLoadComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_LOAD_TRACELOG_RESP* pTraceLogResp = (STOCK_CALC_LOAD_TRACELOG_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_LOAD_TRACE_RESP,
		 sizeof(STOCK_CALC_LOAD_TRACELOG_RESP), NULL, this);
	if (pTraceLogResp == NULL)
		return FALSE;

	pTraceLogResp->respResult = result;
	PostPktByEvent(pTraceLogResp);
	return TRUE;
}

UINT CStockAutoManager::OnStockAutoManagerHisKLineUpdate()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING;
	if (m_jobHisKLineUpdate.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_jobHisKLineUpdate.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		STOCK_CODE_NAME* pCodeName = &m_pJobList->codeName[m_jobHisKLineUpdate.stockIdx];
		STOCK_MANAGER_TRACE_LOG* pTraceLog = &m_pJobTraceLog->traceLog[m_jobHisKLineUpdate.stockIdx + STOCK_AUTO_COUNTS_MAX];

		time_t updateTime = pTraceLog->updateTime;
		time_t hisEndTime = 0;
		int hisUpdateCycles = 0;

		if (updateTime <= 0)
		{
			hisEndTime = m_jobHisKLineUpdate.lastWeekEndTime;
			hisUpdateCycles = -STOCK_HIS_KLINE_MAX_COUNTS;
		}
		else
		{
			QDateTime updateDateTime = QDateTime::fromTime_t(updateTime);
			QDate updateDate = updateDateTime.date();
			int updateOffset = updateDate.dayOfWeek();
			QDate updateEndWeekDate = updateDate.addDays(7 - updateOffset);
			QDate lastWeekDate = QDateTime::fromTime_t(m_jobHisKLineUpdate.lastWeekEndTime).date();
			if (lastWeekDate <= updateEndWeekDate)
				goto _NEXT;
			else
			{
				int lastWeekTime = QDateTime(lastWeekDate).toTime_t();
				hisEndTime = lastWeekTime;
				hisUpdateCycles = -STOCK_HIS_KLINE_MAX_COUNTS;
			}
		}

		STOCK_CALC_UPDATE_HISKLINE* pHisKLineUpdate = (STOCK_CALC_UPDATE_HISKLINE*)m_pUpdateTask->AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_STOCK_HISKLINE,
			sizeof(STOCK_CALC_UPDATE_HISKLINE), MultiTaskEventComplete, this);

		if (pHisKLineUpdate != NULL)
		{
			pHisKLineUpdate->endTime = hisEndTime;
			pHisKLineUpdate->updateCycles = hisUpdateCycles;
			memcpy(pHisKLineUpdate->code, pCodeName->code, sizeof(pHisKLineUpdate->code));
			m_pUpdateTask->PostPktByEvent(pHisKLineUpdate);
			m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else if (m_jobHisKLineUpdate.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = (STOCK_CALC_UPDATE_TRACELOG*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_TRACE_LOG,
			sizeof(STOCK_CALC_UPDATE_TRACELOG), NULL, this);

		if (pUpdateTraceLog != NULL)
		{

			STOCK_MANAGER_TRACE_LOG* pTraceLog = &m_pJobTraceLog->traceLog[m_jobHisKLineUpdate.stockIdx + STOCK_AUTO_COUNTS_MAX];

			pTraceLog->updateTime = m_jobHisKLineUpdate.lastWeekEndTime;
			memcpy(&pUpdateTraceLog->traceLog, pTraceLog, sizeof(STOCK_MANAGER_TRACE_LOG));

			m_pDataTask->PostPktByEvent(pUpdateTraceLog);
		_NEXT:
			m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_NONE;

			if (++m_jobHisKLineUpdate.stockIdx >= m_pJobList->stockCounts)
			{
				m_jobHisKLineUpdate.stockIdx = 0;
				InitStockTraceCalc(&m_pJobTraceLog->traceLog[STOCK_AUTO_COUNTS_MAX], m_pJobList->stockCounts);
				nextStep = STOCK_AUTO_MANAGER_STEP_STOCK_TRACING;
			}
		}
	}
	else
	{
		goto _NEXT;
	}

	if (m_jobHisKLineUpdate.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		ActiveManager();
	float progress = (float)m_jobHisKLineUpdate.stockIdx / m_pJobList->stockCounts;
	m_pAutoWindow->UpdateAutoManagerStep(nextStep, (int)(progress*100));
	return nextStep;
}

void CStockAutoManager::OnHisKLineUpdateResp(STOCK_CALC_UPDATE_HISKLINE_RESP* pHisKLineResp)
{
	if (m_jobHisKLineUpdate.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pHisKLineResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}

	ActiveManager();
}

BOOL CStockAutoManager::OnHisKLineUpdateComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_UPDATE_HISKLINE_RESP* pHisKLineUpdateResp = (STOCK_CALC_UPDATE_HISKLINE_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_STOCK_HISKLINE_RESP,
		sizeof(STOCK_CALC_UPDATE_HISKLINE_RESP), NULL, this);
	if (pHisKLineUpdateResp == NULL)
		return FALSE;

	pHisKLineUpdateResp->respResult = result;

	PostPktByEvent(pHisKLineUpdateResp);
	return TRUE;
}

void CStockAutoManager::OnHisKLineGetResp(STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp)
{
	CStockTraceBase* pTraceBase = pGetHisKLineResp->pTraceBase;
	pTraceBase->OnGetKLineResp(pGetHisKLineResp);
}

BOOL CStockAutoManager::OnHisKLineGetComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_GET_HISKLINE* pGetHisKLine = (STOCK_CALC_GET_HISKLINE*)param;
	CStockTraceBase* pTraceBase = pGetHisKLine->pTraceBase;
	return pTraceBase->OnGetKLineComplete(result, param, paramLen);
}

void CStockAutoManager::OnUpdateTraceLogResp(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp)
{
	CStockTraceBase* pTraceBase = pUpdateTraceLogResp->pTraceBase;
	pTraceBase->OnUpdateTraceLogResp(pUpdateTraceLogResp);
}

BOOL CStockAutoManager::OnUpdateTraceLogComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = (STOCK_CALC_UPDATE_TRACELOG*)param;
	CStockTraceBase* pTraceBase = pUpdateTraceLog->pTraceBase;
	return pTraceBase->OnUpdateTraceLogComplete(result, param, paramLen);
}

void CStockAutoManager::OnCurHisKLineGetResp(STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp)
{
	CStockTraceBase* pTraceBase = pGetCurHisKLineResp->pTraceBase;
	pTraceBase->OnGetCurKLineResp(pGetCurHisKLineResp);
}

BOOL CStockAutoManager::OnCurHisKLineGetComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_GET_CUR_HISKLINE* pGetHisKLine = (STOCK_CALC_GET_CUR_HISKLINE*)param;
	CStockTraceBase* pTraceBase = pGetHisKLine->pTraceBase;
	return pTraceBase->OnGetCurKLineComplete(result, param, paramLen);
}

void CStockAutoManager::OnInsertTraceRecordResp(STOCK_CALC_INSERT_TRACERECORD_RESP* pInsertTraceRecordResp)
{
	CStockTraceBase* pTraceBase = pInsertTraceRecordResp->pTraceBase;
	pTraceBase->OnInsertTraceRecordResp(pInsertTraceRecordResp);
}

BOOL CStockAutoManager::OnInsertTraceRecordComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_INSERT_TRACERECORD* pUpdateTraceLog = (STOCK_CALC_INSERT_TRACERECORD*)param;
	CStockTraceBase* pTraceBase = pUpdateTraceLog->pTraceBase;
	return pTraceBase->OnInsertTraceRecordComplete(result, param, paramLen);
}

void CStockAutoManager::OnUpdateConfigTrace(STOCK_CALC_UPDATE_CONFIG_TRACE* pUpdateConfigTrace)
{
	memcpy(&m_traceConfig, &pUpdateConfigTrace->configTrace, sizeof(STOCKAUTO_CONFIG_TRACE));
	if (m_pTraceWeek != NULL)
		m_pTraceWeek->UpdateConfigTrace(&m_traceConfig);

	if (m_pTraceReal != NULL)
		m_pTraceReal->UpdateConfigTrace(&m_traceConfig);
}

void CStockAutoManager::OnClearTraceHistimeResp(STOCK_CALC_CLEAR_TRACE_HISTIME_RESP* pClearResp)
{
	if (pClearResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_jobResetTrace.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobResetTrace.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}

	ActiveManager();
}

BOOL CStockAutoManager::OnClearTraceHistimeComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_CLEAR_TRACE_HISTIME_RESP* pClearResp = (STOCK_CALC_CLEAR_TRACE_HISTIME_RESP*)AllocPktByEvent(STOCK_CALC_EVENT_CLEAR_TRACE_HISTIME_RESP,
		sizeof(STOCK_CALC_CLEAR_TRACE_HISTIME_RESP), NULL, this);
	if (pClearResp == NULL)
		return FALSE;

	pClearResp->respResult = result;
	PostPktByEvent(pClearResp);
	return TRUE;
}

UINT CStockAutoManager::OnStockAutoManagerResetTrace()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_RESET_TRACE;
	if (m_jobResetTrace.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
	{
		return nextStep;
	}

	if (m_jobResetTrace.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		STOCK_CALC_CLEAR_TRACE_HISTIME* pClearHisTime = (STOCK_CALC_CLEAR_TRACE_HISTIME*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_CLEAR_TRACE_HISTIME,
			sizeof(STOCK_CALC_CLEAR_TRACE_HISTIME), MultiTaskEventComplete, this);
		if (pClearHisTime == NULL)
			return nextStep;

		m_pDataTask->PostPktByEvent(pClearHisTime);
	}
	else
	{
		m_pTraceWeek->ResetTrace();
		m_pTraceReal->ResetTrace();

		m_jobResetTrace.jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = m_jobResetTrace.lastEventStep;

		m_pAutoWindow->UpdateResetTraceResult(1);
		ActiveManager();
	}

	return nextStep;
}

void CStockAutoManager::OnResetTraceCalc(STOCK_CALC_RESET_TRACE_CALC* pResetTrace)
{
	if (m_managerStep < STOCK_AUTO_MANAGER_STEP_STOCK_TRACING)
		return;

	m_jobResetTrace.jobStep = TASK_EVENT_JOB_STEP_NONE;
	m_jobResetTrace.lastEventStep = m_managerStep;
	m_managerStep = STOCK_AUTO_MANAGER_STEP_RESET_TRACE;
	ActiveManager();
}

int CStockAutoManager::OnQueryTraceInfo(QT_STOCK_TRACEINFO_QUERY_PARAM* pQueryTraceInfo)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = &m_pJobTraceLog->traceLog[STOCK_AUTO_COUNTS_MAX];
	int logCounts = m_pJobList->stockCounts;

	for (int i = 0; i < logCounts; i++, pTraceLog++)
	{
		if (strcmp(pTraceLog->code, pQueryTraceInfo->code) == 0)
		{
			pQueryTraceInfo->traceStep = pTraceLog->traceStep;
			pQueryTraceInfo->highTime = pTraceLog->highTime;
			pQueryTraceInfo->fHighVal = pTraceLog->fHighVal;
			pQueryTraceInfo->buyTime = pTraceLog->buyTime;
			pQueryTraceInfo->fBuyVal = pTraceLog->fBuyVal;
			pQueryTraceInfo->topTime = pTraceLog->topTime;
			pQueryTraceInfo->fTopVal = pTraceLog->fTopVal;
			pQueryTraceInfo->sellTime = pTraceLog->sellTime;
			pQueryTraceInfo->fSellVal = pTraceLog->sellTime;
			return EVENT_COMPLETE_OK;
		}
	}

	return EVENT_COMPLETE_FAIL;
}

UINT CStockAutoManager::OnStockAutoManagerTrace()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_STOCK_TRACING;
	if (m_pTraceWeek != NULL)
	{
		m_pTraceWeek->Trace();
	}


	return nextStep;
}