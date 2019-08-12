/*
*stockAutoManager.cpp
*/

#include "../include/stockAutoManager.h"

CStockAutoManager::CStockAutoManager()
{
	m_managerStep = STOCK_AUTO_MANAGER_STEP_NONE;
	m_pUpdateTask = NULL;
	m_pJobList = NULL;
	m_pDataTask = NULL;
	m_pJobTraceLog = NULL;
	memset(&m_jobListUpdate, 0, sizeof(m_jobListUpdate));
}

CStockAutoManager::~CStockAutoManager()
{
	Close();
}

BOOL CStockAutoManager::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize)
{
	int jobListSize = sizeof(STOCK_MANAGER_JOB_LIST) + STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_CODE_NAME);
	int jobTraceLogSize = sizeof(STOCK_MANAGER_JOB_TRACELOG_LOAD) + STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_MANAGER_TRACE_LOG);
	do
	{
		m_pDataTask = new CStockDataTask();
		if (m_pDataTask == NULL)
			break;

		if (!m_pDataTask->Create(STOCK_DATA_TASK_NAME, STOCK_DATA_TASK_SZ, STOCK_DATA_TASK_PRI, 0,
			STOCK_DATA_TASK_TIMEOUT, STOCK_DATA_TASK_EVENTS, STOCK_UPDATE_TASK_EVENT_PARAM_SIZE))
			break;

		m_pJobList = (STOCK_MANAGER_JOB_LIST*)calloc(1, jobListSize);
		if (m_pJobList == NULL)
			break;

		m_pJobTraceLog = (STOCK_MANAGER_JOB_TRACELOG_LOAD*)calloc(1, sizeof(jobTraceLogSize));
		if (m_pJobTraceLog == NULL)
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
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_ERROR:
		break;

	case STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING:
		OnStockAutoManagerTraceLogLoad();
		break;
	}

	return TRUE;
}

int CStockAutoManager::OnEventActive(UINT cmd, void* param, int paramLen)
{
	int result = EVENT_COMPLETE_OK;
	STOCK_CALC_GET_LIST_RESP* pGetListResp = (STOCK_CALC_GET_LIST_RESP*)param;
	STOCK_CALC_UPDATE_LIST_RESP* pUpdateListResp = (STOCK_CALC_UPDATE_LIST_RESP*)param;
	STOCK_CALC_LOAD_TRACELOG_RESP* pLoadTraceLogResp = (STOCK_CALC_LOAD_TRACELOG_RESP*)param;
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
	}

	return re;
}



UINT CStockAutoManager::OnStockAutoManagerInit()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_LIST_INIT;
	if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// 查询
		STOCK_CALC_GET_LIST getListParam;
		InitMultiTaskEventParam(&getListParam, sizeof(getListParam));

		getListParam.bufCounts = STOCK_AUTO_COUNTS_MAX;
		getListParam.pListBuf = m_pJobList->codeName;
		BOOL postReuslt = m_pDataTask->PostEvent(STOCK_CALC_EVENT_GET_STOCK_LIST, &getListParam,
			sizeof(getListParam), MultiTaskEventComplete);

		if (postReuslt)
		{
			m_pJobList->jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING;
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
	STOCK_CALC_GET_LIST_RESP getListResp;
	memset(&getListResp, 0, sizeof(STOCK_CALC_GET_LIST_RESP));
	getListResp.respResult = result;

	return PostEvent(STOCK_CALC_EVENT_GET_STOCK_LIST_RESP, &getListResp, sizeof(getListResp), NULL);
}


UINT CStockAutoManager::OnStockAutoManagerListUpdate()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_LIST_UPDATING;
	if (m_jobListUpdate.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_jobListUpdate.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// 查询
		STOCK_CALC_UPDATE_LIST updateListParam;
		InitMultiTaskEventParam(&updateListParam, sizeof(updateListParam));


		BOOL postReuslt = m_pUpdateTask->PostEvent(STOCK_CALC_EVENT_UPDATE_STOCK_LIST, &updateListParam,
			sizeof(updateListParam), MultiTaskEventComplete);

		if (postReuslt)
		{
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
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_ERROR;
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
	STOCK_CALC_UPDATE_LIST_RESP updateListResp;
	memset(&updateListResp, 0, sizeof(STOCK_CALC_GET_LIST_RESP));
	updateListResp.respResult = result;

	return PostEvent(STOCK_CALC_EVENT_UPDATE_STOCK_LIST_RESP, &updateListResp, sizeof(updateListResp), NULL);
}

UINT CStockAutoManager::OnStockAutoManagerTraceLogLoad()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING;
	if (m_pJobTraceLog->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return nextStep;

	if (m_pJobTraceLog->jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// 查询
		STOCK_CALC_LOAD_TRACELOG traceLogLoadParam;
		InitMultiTaskEventParam(&traceLogLoadParam, sizeof(traceLogLoadParam));


		BOOL postReuslt = m_pUpdateTask->PostEvent(STOCK_CALC_EVENT_LOAD_TRACE_LOG, &traceLogLoadParam,
			sizeof(traceLogLoadParam), MultiTaskEventComplete);

		if (postReuslt)
		{
			m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		}

	}
	else if (m_pJobTraceLog->jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING;
	}
	else
	{
		m_pJobTraceLog->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_ERROR;
	}

	return nextStep;
}

void CStockAutoManager::OnTraceLogLoadResp(STOCK_CALC_LOAD_TRACELOG_RESP* pTraceLogResp)
{
	if (pTraceLogResp->respResult == EVENT_COMPLETE_FAIL)
	{
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
	STOCK_CALC_LOAD_TRACELOG_RESP traceLogLoadResp;
	memset(&traceLogLoadResp, 0, sizeof(STOCK_CALC_GET_LIST_RESP));
	traceLogLoadResp.respResult = result;

	return PostEvent(STOCK_CALC_EVENT_LOAD_TRACE_RESP, &traceLogLoadResp, sizeof(traceLogLoadResp), NULL);
}