/*
*stockAutoManager.cpp
*/
#include <qdatetime.h>
#include "../include/stockTraceBase.h"
#include "../include/stockAutoManager.h"

CStockAutoManager::CStockAutoManager()
{
	m_managerStep = STOCK_AUTO_MANAGER_STEP_NONE;
	m_pUpdateTask = NULL;
	m_pJobList = NULL;
	m_pDataTask = NULL;
	m_pJobTraceLog = NULL;
	memset(&m_jobListUpdate, 0, sizeof(m_jobListUpdate));
	memset(&m_jobHisKLineUpdate, 0, sizeof(m_jobHisKLineUpdate));
	dllInit(&m_listTraceWeek);
	dllInit(&m_listTraceReal);
}

CStockAutoManager::~CStockAutoManager()
{
	Close();
}

BOOL CStockAutoManager::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize)
{
	int jobListSize = sizeof(STOCK_MANAGER_JOB_LIST) + STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_CODE_NAME);
	int jobTraceLogSize = sizeof(STOCK_MANAGER_JOB_TRACELOG_LOAD) + 2*STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_MANAGER_TRACE_LOG);
	do
	{
		m_pDataTask = new CStockDataTask();
		if (m_pDataTask == NULL)
			break;

		if (!m_pDataTask->Create(STOCK_DATA_TASK_NAME, STOCK_DATA_TASK_SZ, STOCK_DATA_TASK_PRI, 0,
			STOCK_DATA_TASK_TIMEOUT, STOCK_DATA_TASK_EVENTS, STOCK_UPDATE_TASK_EVENT_PARAM_SIZE))
			break;

		m_pJobList = (STOCK_MANAGER_JOB_LIST*)calloc(STOCK_AUTO_COUNTS_MAX, jobListSize);
		if (m_pJobList == NULL)
			break;

		m_pJobTraceLog = (STOCK_MANAGER_JOB_TRACELOG_LOAD*)calloc(1, jobTraceLogSize);
		if (m_pJobTraceLog == NULL)
			break;

		m_pCalcTraceBuf = (STOCK_CALC_TRACE_NODE*)calloc(1, sizeof(STOCK_CALC_TRACE_NODE));		
		if (m_pCalcTraceBuf == NULL)
			break;

		m_pJobList->stockCounts = STOCK_AUTO_COUNTS_MAX;
		m_pUpdateTask = new CStockUpdateTask();
		if (m_pUpdateTask == NULL)
			break;

		if (!m_pUpdateTask->Create(STOCK_DATA_TASK_NAME, STOCK_DATA_TASK_SZ, STOCK_DATA_TASK_PRI, 0,
			STOCK_DATA_TASK_TIMEOUT, STOCK_DATA_TASK_EVENTS, STOCK_UPDATE_TASK_EVENT_PARAM_SIZE))
			break;

		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents, maxEventParamSize))
			break;

		m_managerStep = STOCK_AUTO_MANAGER_STEP_LIST_INIT;
		return TRUE;
	} while (FALSE);

	Close();
	return FALSE;

}

void CStockAutoManager::Close()
{
	CMultiEventsTask::Close();

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
}

void CStockAutoManager::OnExit()
{
	m_pUpdateTask->StopWork();
	m_pUpdateTask->WaitWorkExit(2000);
	m_pDataTask->StopWork();
	m_pDataTask->WaitWorkExit(2000);
}

BOOL CStockAutoManager::CheckSelf()
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
	
		break;
	}

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

	case STOCK_CALC_EVENT_GET_STOCK_HISKLINE:
		re = OnHisKLineGetComplete(result, param, paramLen);
		break;
	}

	return re;
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
		STOCK_CALC_TRACE_NODE*	m_pTraceNode;
	};

	m_pTraceNode = m_pCalcTraceBuf;
	for (int j = 0; j < logCounts; j++, pTraceLog++, m_pTraceNode++)
	{
		m_pTraceNode->stockIdx = j;
		m_pTraceNode->pTraceLog = pTraceLog;

		if (pTraceLog->traceStep < CALC_STOCK_TRADE_STEP_CHECK_HIGH)
			dllAdd(&m_listTraceWeek, pNode);
		else
			dllAdd(&m_listTraceReal, pNode);
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

void CStockAutoManager::PostGetHisKLineRespPkt(STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLine)
{
	PostPktByEvent(pGetHisKLine);
}

STOCK_CALC_UPDATE_TRACELOG* CStockAutoManager::AllocUpdateTraceLogPkt(CStockTraceBase* pTraceBase)
{
	STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = (STOCK_CALC_UPDATE_TRACELOG*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_TRACE_LOG, sizeof(STOCK_CALC_UPDATE_TRACELOG),
		NULL, this);
	if (pUpdateTraceLog == NULL)
		return NULL;

	pUpdateTraceLog->pRsv = pTraceBase;
	return pUpdateTraceLog;
}

void CStockAutoManager::PostUpdateTraceLogPkt(STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog)
{
	m_pDataTask->PostPktByEvent(pUpdateTraceLog);
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

	return nextStep;
}

void CStockAutoManager::OnStockListGetResp(STOCK_CALC_GET_LIST_RESP* pGetListResp)
{
	if (pGetListResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_pJobList->stockCounts = pGetListResp->respResult;
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
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

	return nextStep;
}


void CStockAutoManager::OnStockListUpdateResp(STOCK_CALC_UPDATE_LIST_RESP* pUpdateListResp)
{
	if (pUpdateListResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobListUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
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
	else if (m_pJobTraceLog->jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
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
	else
	{
		m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_NONE;
		InitStockTraceByLog(m_pJobTraceLog, m_pJobList);
		nextStep = STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING;
	}

	return nextStep;
}

void CStockAutoManager::OnTraceLogLoadResp(STOCK_CALC_LOAD_TRACELOG_RESP* pTraceLogResp)
{
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

		if (pHisKLineUpdate == NULL)
			return nextStep;
		pHisKLineUpdate->endTime = hisEndTime;
		pHisKLineUpdate->updateCycles = hisUpdateCycles;
		memcpy(pHisKLineUpdate->code, pCodeName->code, sizeof(pHisKLineUpdate->code));
		m_pUpdateTask->PostPktByEvent(pHisKLineUpdate);
		m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
	}
	else if (m_jobHisKLineUpdate.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = (STOCK_CALC_UPDATE_TRACELOG*)m_pDataTask->AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_TRACE_LOG,
			sizeof(STOCK_CALC_UPDATE_TRACELOG), NULL, this);

		if (pUpdateTraceLog == NULL)
			return nextStep;
		
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
	else
	{
		goto _NEXT;
	}

	return nextStep;
}

void CStockAutoManager::OnHisKLineUpdateResp(STOCK_CALC_UPDATE_HISKLINE_RESP* pHisKLineResp)
{
	if (pHisKLineResp->respResult == EVENT_COMPLETE_FAIL)
	{
		m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobHisKLineUpdate.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
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