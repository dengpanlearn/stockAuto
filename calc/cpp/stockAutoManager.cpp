/*
*stockAutoManager.cpp
*/

#include "../include/stockAutoManager.h"

CStockAutoManager::CStockAutoManager()
{
	m_managerStep = STOCK_AUTO_MANAGER_STEP_NONE;
	m_pUpdateTask = NULL;
}

CStockAutoManager::~CStockAutoManager()
{

}

BOOL CStockAutoManager::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents)
{
	m_pUpdateTask = new CStockUpdateTask();
	if (m_pUpdateTask == NULL)
		return FALSE;

	if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents))
	{
		delete m_pUpdateTask;
		m_pUpdateTask = NULL;
		return FALSE;
	}

	return TRUE;
}

void CStockAutoManager::Close()
{
	CMultiEventsTask::Close();
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