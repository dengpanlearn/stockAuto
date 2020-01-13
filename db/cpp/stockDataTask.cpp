/*
*stockDataSqlite.cpp*/

#include <qdir.h>
#include <qtGlobal.h>
#include <qtextcodec.h>
#include <qsqlquery.h>
#include <qtStockAgent.h>
#include <qtStockTraceDef.h>
#include <stockConfigTask.h>
#include "../include/stockDataSqlite.h"
#include "../include/stockDataTask.h"

/*EXTERN*/
extern CStockConfigTask g_configTask;

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

		if (!pStockSqlite->Init(m_configData.dataDir, m_configData.listFileName, m_configData.klineFileName, m_configData.tracelogFileName, m_configData.traceRecordFileName))
			break;

		if (!CMultiEventsTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs, maxEvents, maxEventParamSize))
			break;

		pStockSqlite->TraceLogIsOn();
		pStockSqlite->TraceRecordIsOn();
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

BOOL CStockDataTask::UpdateConfigData()
{
	STOCK_CALC_UPDATE_CONFIG_DATA* pUpdateConfigData = (STOCK_CALC_UPDATE_CONFIG_DATA*)AllocPktByEvent(STOCK_CALC_EVENT_UPDATE_CONFIG_DATA,
		sizeof(STOCK_CALC_UPDATE_CONFIG_DATA), NULL, NULL);

	if (pUpdateConfigData == NULL)
		return FALSE;

	PostPktByEvent(pUpdateConfigData);
	return TRUE;
}

void CStockDataTask::OnActive()
{
	CMultiEventsTask::OnActive();
}

int CStockDataTask::OnEventActive(UINT cmd, void* param, int paramLen)
{
	int result = EVENT_COMPLETE_OK;
	union 
	{
		STOCK_CALC_GET_LIST * pGetList;
		STOCK_CALC_LOAD_TRACELOG* pLoadTraceLog;
		STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog;
		STOCK_CALC_GET_HISKLINE* pGetHisKLine;
		QT_STOCK_HISKLINE_QUERY_PARAM* pQueryHisKLine;
		STOCK_CALC_UPDATE_CONFIG_DATA* pUpdateConfigData;
		STOCK_CALC_INSERT_TRACERECORD* pInsertTraceRecord;
		QT_STOCK_TRACERECORD_QUERY_PARAM* pQueryTraceRecord;
	};

	pGetList = (STOCK_CALC_GET_LIST*)param;

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

	case STOCK_CALC_EVENT_UPDATE_TRACE_LOG:
		result = m_pStockData->UpdateTraceLog(&pUpdateTraceLog->traceLog);
		break;

	case STOCK_CALC_EVENT_GET_STOCK_HISKLINE:
		result = m_pStockData->GetHisKLine(pGetHisKLine->code, pGetHisKLine->pKLineBuf, pGetHisKLine->getCnt);
		break;

	case STOCK_QT_EVENT_QUERY_STOCK_HISKLINE:
		result = m_pStockData->GetHisKLine(pQueryHisKLine->code, pQueryHisKLine->pHisKLineBuf, pQueryHisKLine->hisCounts);
		break;

	case STOCK_CALC_EVENT_UPDATE_CONFIG_DATA:
		result = EVENT_COMPLETE_OK;
		break;

	case STOCK_CALC_EVENT_CLEAR_TRACE_HISTIME:
		result = m_pStockData->ClearTraceLogHisTime();
		break;

	case STOCK_CALC_EVENT_INSERT_TRACE_RECORD:
		result = m_pStockData->InsertTraceRecord(&pInsertTraceRecord->traceLog);
		break;

	case STOCK_QT_EVENT_QUERY_TRACE_RECORD:
		result = m_pStockData->GetTraceRecord(pQueryTraceRecord->pTraceRecordInfo, pQueryTraceRecord->startTime, pQueryTraceRecord->endTime,
			pQueryTraceRecord->counts, pQueryTraceRecord->offset);
		break;

	default:
		result = EVENT_COMPLETE_FAIL;
	}


	return result;
}

void CStockDataTask::InitConfig(STOCKAUTO_CONFIG_DATA* pConfigData)
{
	// 从配置文件中读取
	g_configTask.GetConfigData(pConfigData);
#if 0
	if (pConfigData->dataDir[0] == '\0')
		memcpy(pConfigData->dataDir, STOCKAUTO_CONFIG_DATA_DATADIR_DFT, sizeof(STOCKAUTO_CONFIG_DATA_DATADIR_DFT));

	if (pConfigData->listFileName[0] == '\0')
		memcpy(pConfigData->listFileName, STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT, sizeof(STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT));

	if (pConfigData->klineFileName[0] == '\0')
		memcpy(pConfigData->klineFileName, STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT, sizeof(STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT));


	if (pConfigData->tracelogFileName[0] == '\0')
		memcpy(pConfigData->tracelogFileName, STOCKAUTO_CONFIG_DATA_TRACELOG_FILENAME_DFT, sizeof(STOCKAUTO_CONFIG_DATA_TRACELOG_FILENAME_DFT));
#endif


}