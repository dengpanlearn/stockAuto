/*
*stockDataSqlite.cpp*/

#include <qdir.h>
#include <qtGlobal.h>
#include <qtextcodec.h>
#include <qsqlquery.h>
#include "../include/stockDataSqlite.h"
#include "../include/stockDataTask.h"

CStockDataTask::CStockDataTask()
{
	m_pStockData = NULL;
	memset(&m_configData, 0, sizeof(m_configData));
}

CStockDataTask::~CStockDataTask()
{
	Close();
}

BOOL CStockDataTask::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize)
{
	InitConfig(&m_configData);
	CStockDataSqlite* pStockSqlite = new CStockDataSqlite();
	if (pStockSqlite == NULL)
		return FALSE;
	do
	{

		if (!pStockSqlite->Init(m_configData.dataDir, m_configData.listFileName, m_configData.klineFileName, m_configData.tracelogFileName))
			break;

		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents, maxEventParamSize))
			break;

		m_pStockData = pStockSqlite;
		return TRUE;
	} while (FALSE);

	Close();
	return FALSE;

}


void CStockDataTask::Close()
{
	CMultiEventsTask::Close();

	if (m_pStockData != NULL)
	{
		delete m_pStockData;
		m_pStockData = NULL;
	}
}

void CStockDataTask::OnActive()
{
	CMultiEventsTask::OnActive();
}

int CStockDataTask::OnEventActive(UINT cmd, void* param, int paramLen)
{
	int result = EVENT_COMPLETE_OK;
	STOCK_CALC_GET_LIST * pGetList = (STOCK_CALC_GET_LIST*)param;
	STOCK_CALC_LOAD_TRACELOG* pLoadTraceLog = (STOCK_CALC_LOAD_TRACELOG*)param;
	switch (cmd)
	{
	case STOCK_CALC_EVENT_GET_STOCK_LIST:
		if (!m_pStockData->StockListIsOn())
		{
			result = EVENT_COMPLETE_FAIL;
		}
		else
		{
			result = m_pStockData->GetStockList(pGetList->pListBuf, pGetList->bufCounts);
		}
		break;

	case STOCK_CALC_EVENT_LOAD_TRACE_LOG:
		if (!m_pStockData->TraceLogIsOn())
		{
			result = EVENT_COMPLETE_FAIL;
		}
		else
		{
			result = m_pStockData->GetTraceLog(pLoadTraceLog->pLogBuf, pLoadTraceLog->bufCounts);
		}
		break;
	default:
		result = EVENT_COMPLETE_FAIL;
	}


	return result;
}

void CStockDataTask::InitConfig(STOCKAUTO_CONFIG_DATA* pConfigData)
{
	// 从配置文件中读取
	if (pConfigData->dataDir[0] == '\0')
		memcpy(pConfigData->dataDir, STOCKAUTO_CONFIG_DATA_DATADIR_DFT, sizeof(STOCKAUTO_CONFIG_DATA_DATADIR_DFT));

	if (pConfigData->listFileName[0] == '\0')
		memcpy(pConfigData->listFileName, STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT, sizeof(STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT));

	if (pConfigData->klineFileName[0] == '\0')
		memcpy(pConfigData->klineFileName, STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT, sizeof(STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT));


	if (pConfigData->tracelogFileName[0] == '\0')
		memcpy(pConfigData->tracelogFileName, STOCKAUTO_CONFIG_DATA_TRACELOG_FILENAME_DFT, sizeof(STOCKAUTO_CONFIG_DATA_TRACELOG_FILENAME_DFT));
}