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
}

CStockAutoManager::~CStockAutoManager()
{
	Close();
}

BOOL CStockAutoManager::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize)
{
	int jobListSize = sizeof(STOCK_MANAGER_JOB_LIST) + STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_CODE_NAME);
	do
	{
		m_pDataTask = new CStockDataTask();
		if (m_pDataTask == NULL)
			break;

		m_pJobList = (STOCK_MANAGER_JOB_LIST*)calloc(1, jobListSize);
		if (m_pJobList == NULL)
			break;

		m_pJobList->stockCounts = STOCK_AUTO_COUNTS_MAX;
		m_pUpdateTask = new CStockUpdateTask();
		if (m_pUpdateTask == NULL)
			break;

		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents, maxEventParamSize))
			break;


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

BOOL CStockAutoManager::CheckSelf()
{
	switch (m_managerStep)
	{
	case STOCK_AUTO_MANAGER_STEP_NONE:
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_PREPARED:
		break;


	case STOCK_AUTO_MANAGER_STEP_LIST_UPDATING:
		break;

	case STOCK_AUTO_MANAGER_STEP_KLINE_PREPARED:
		break;

	case STOCK_AUTO_MANAGER_STEP_CALC_LOADING:
		break;
	}
}

int CStockAutoManager::OnEventActive(UINT cmd, void* param, int paramLen)
{
	switch (cmd)
	{
	case STOCK_CALC_EVENT_GET_STOCK_LIST_RESP:

		break;


	}
}

BOOL CStockAutoManager::OnEventComplete(UINT cmd, int result, void* param, int paramLen)
{
	BOOL re = TRUE;
	switch (cmd)
	{
	case STOCK_CALC_EVENT_GET_STOCK_LIST:
		re = OnStockListGetComplete(result, param, paramLen);
		break;


	}

	return re;
}



UINT CStockAutoManager::OnStockAutoManagerInit()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_NONE;
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
			m_pJobList->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_PREPARED;
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
	getListResp.eventParam.pMultiEventTask = NULL;
	getListResp.respResult = result;

	return PostEvent(STOCK_CALC_EVENT_GET_STOCK_LIST_RESP, &getListResp, sizeof(getListResp), NULL);
}


UINT CStockAutoManager::OnStockAutoManagerListUpdate()
{
	UINT nextStep = STOCK_AUTO_MANAGER_STEP_NONE;
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
			m_pJobList->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP;
		}
	}
	else if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK) // 获取成功
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_PREPARED;
	}
	else
	{
		m_pJobList->jobStep = TASK_EVENT_JOB_STEP_NONE;
		nextStep = STOCK_AUTO_MANAGER_STEP_LIST_UPDATING;
	}

	return nextStep;
}