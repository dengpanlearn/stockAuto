/*
*stockUpdateTask.cpp
*/

#include "../include/stockCalcDef.h"
#include "../include/stockXueqiuUpdate.h"
#include "../include/stockUpdateTask.h"

CStockUpdateTask::CStockUpdateTask()
{
	m_pStockUpdate = NULL;
	memset(&m_configPython, 0, sizeof(m_configPython));
}

CStockUpdateTask::~CStockUpdateTask()
{
	Close();
}

BOOL CStockUpdateTask::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents)
{
	InitConfig(&m_configPython);
	CStockXiuQiuUpdate* pXueQiuUpdate = new CStockXiuQiuUpdate();
	if (pXueQiuUpdate == NULL)
		return FALSE;

	do
	{
		if (!pXueQiuUpdate->Create(m_configPython.updateDir, m_configPython.dbStockList, m_configPython.dbStockKLine,
			m_configPython.moduleDir, m_configPython.klineModule))
			break;

		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents))
			break;

		m_pStockUpdate = pXueQiuUpdate;

		return TRUE;
	} while (FALSE);


	Close();

	return FALSE;
}

void CStockUpdateTask::Close()
{
	// 先调用基类的Close 函数
	CMultiEventsTask::Close();
	if (m_pStockUpdate != NULL)
	{
		delete m_pStockUpdate;
		m_pStockUpdate = NULL;
	}
}

void CStockUpdateTask::OnActive()
{
	CMultiEventsTask::OnActive();
}

int CStockUpdateTask::OnEventActive(UINT cmd, void* param)
{
	BOOL result = TRUE;
	

	switch (cmd)
	{
	case STOCK_CALC_EVENT_UPDATE_STOCK_LIST:
		result = m_pStockUpdate->UpdateLatestStockList();
		break;

	case STOCK_CALC_EVENT_UPDATE_STOCK_KLINE:
		STOCK_CALC_UPDATE_KLINE* pKLineParam = (STOCK_CALC_UPDATE_KLINE*)param;
		result = m_pStockUpdate->UpdateLatestKLine(pKLineParam->codeName, pKLineParam->updateCycles);
		break;
	}


	return result? EVENT_COMPLETE_OK: EVENT_COMPLETE_FAIL;
}

void CStockUpdateTask::InitConfig(STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	// 从配置文件中读取
	if (pConfigPython->moduleDir[0] == '\0')
		memcpy(pConfigPython->moduleDir, STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT, sizeof(STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT));
	
	if (pConfigPython->klineModule[0] == '\0')
		memcpy(pConfigPython->klineModule, STOCKAUTO_CONFIG_PYTHON_KLINE_MODULE_DFT, sizeof(STOCKAUTO_CONFIG_PYTHON_KLINE_MODULE_DFT));

	if (pConfigPython->updateDir[0] == '\0')
		memcpy(pConfigPython->updateDir, STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR, sizeof(STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR));

	if (pConfigPython->dbStockList[0] == '\0')
		memcpy(pConfigPython->dbStockList, STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST, sizeof(STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST));


	if (pConfigPython->dbStockKLine[0] == '\0')
		memcpy(pConfigPython->dbStockKLine, STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE, sizeof(STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE));
}