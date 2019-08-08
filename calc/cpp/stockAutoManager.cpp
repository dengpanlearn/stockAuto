/*
*stockAutoManager.cpp
*/

#include "../include/stockAutoManager.h"

CStockAutoManager::CStockAutoManager()
{
	m_managerStep = STOCK_AUTO_MANAGER_STEP_NONE;
	m_pUpdateTask = NULL;
	m_pJobList = NULL;
}

CStockAutoManager::~CStockAutoManager()
{

}

BOOL CStockAutoManager::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents)
{
	int jobListSize = sizeof(STOCK_MANAGER_JOB_LIST) + STOCK_AUTO_COUNTS_MAX * sizeof(STOCK_CODE_NAME);
	do
	{
		m_pJobList = (STOCK_MANAGER_JOB_LIST*)calloc(1, jobListSize);
		if (m_pJobList == NULL)
			break;

		m_pUpdateTask = new CStockUpdateTask();
		if (m_pUpdateTask == NULL)
			break;

		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents))
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
}

BOOL CStockAutoManager::CheckSelf()
{
	switch (m_managerStep)
	{
	case STOCK_AUTO_MANAGER_STEP_NONE:
		break;

	case STOCK_AUTO_MANAGER_STEP_LIST_PREPARED:
		break;

	case STOCK_AUTO_MANAGER_STEP_KLINE_PREPARED:
		break;

	case STOCK_AUTO_MANAGER_STEP_CALC_LOADING:
		break;
	}
}

int CStockAutoManager::OnEventActive(UINT cmd, void* param)
{

}

UINT CStockAutoManager::OnStockAutoManagerInit()
{
	if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return TRUE;

	if (m_pJobList->jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		// ≤È—Ø
	}
}