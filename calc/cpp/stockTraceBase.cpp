/*
*stockTraceBase.cpp
*/

#include <qtStockTraceDef.h>
#include "../include/stockConfigDef.h"
#include "../include/stockTraceBase.h"

CStockTraceBase::CStockTraceBase(CStockAutoManager* pAutoManager, DL_LIST* pTraceList)
{
	m_pAutoManager = pAutoManager;
	m_pTraceList = pTraceList;

	memset(&m_jobGetHisKine, 0, sizeof(m_jobGetHisKine));
	memset(&m_jobUpdateTraceLog, 0, sizeof(m_jobUpdateTraceLog));
	memset(&m_jobGetCurHisKLine, 0, sizeof(m_jobGetCurHisKLine));
}

CStockTraceBase::~CStockTraceBase()
{
	Close();
}

BOOL CStockTraceBase::Init(int hisKLineCounts)
{
	m_pHisKLine = (STOCK_CALC_TRACE_KLINE*)calloc(hisKLineCounts, sizeof(STOCK_CALC_TRACE_KLINE));
	if (m_pHisKLine == NULL)
		return FALSE;

	m_pCurNode = DLL_FIRST(m_pTraceList);
	union
	{
		STOCK_CALC_TRACE_NODE* pTraceNode;
		DL_NODE*			pNode;
	};

	pNode = m_pCurNode;
	m_workStep = STOCK_TRACE_STEP_PRPARING;
	while (pNode)
	{
		InitStockTrace(pTraceNode);
		pNode = pNode->next;
	}

	m_hisKLineCounts = hisKLineCounts;
	return TRUE;
}

void CStockTraceBase::Close()
{
	if (m_pHisKLine != NULL)
	{
		free(m_pHisKLine);
		m_pHisKLine = NULL;
	}
}

void CStockTraceBase::ResetTrace()
{
	m_pCurNode = DLL_FIRST(m_pTraceList);
	union
	{
		STOCK_CALC_TRACE_NODE* pTraceNode;
		DL_NODE*			pNode;
	};
	DL_NODE* pNextNode;
	pNode = m_pCurNode;
	m_workStep = STOCK_TRACE_STEP_PRPARING;
	while (pNode)
	{ 
		pNextNode = pNode->next;
		ResetStockTrace(pTraceNode);
		InitStockTrace(pTraceNode);
		pNode = pNextNode;
	}

	m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_NONE;
	m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_NONE;
	m_jobGetCurHisKLine.jobStep = TASK_EVENT_JOB_STEP_NONE;
}

void CStockTraceBase::Trace()
{
	if (m_pCurNode == NULL)
		return;

	
	STOCK_CALC_TRACE_NODE*  pTraceNode = (STOCK_CALC_TRACE_NODE*)m_pCurNode;

	if (m_workStep == STOCK_TRACE_STEP_PRPARING)
	{
		m_workStep = DoPrepareWork(pTraceNode);
	}
	else if (m_workStep == STOCK_TRACE_STEP_WORKING)
	{
		m_workStep = DoTraceWork(pTraceNode);
	}
	else if (m_workStep == STOCK_TRACE_STEP_UPDATING)
	{
		m_workStep = DoTraceUpdate(pTraceNode);
	}
	else
	{
		m_workStep = Next(m_pCurNode);
	}
	
	if (IsActiveManager())
		m_pAutoManager->ActiveManager();
}

void CStockTraceBase::AddTraceStock(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	dllAdd(m_pTraceList, &pTraceNode->node);
	if (m_pCurNode == NULL)
	{
		m_pCurNode = &pTraceNode->node;

		if (IsActiveManagerAfterAddNode())
			m_pAutoManager->ActiveManager();
	}
}

void CStockTraceBase::TraceStock(STOCK_CALC_TRACE_NODE* pTraceNode)
{

}

void CStockTraceBase::UpdateStockTraceStat(int stockIdx, char const* pCode, UINT stat)
{
	m_pAutoManager->UpdateStockTraceStat(stockIdx, pCode, stat);
}

void CStockTraceBase::InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_HIGH)
	{
		UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_RAISE_BALANCED | QT_STOCK_TRACE_LOG_STAT_ADD);
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
	{
		UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_HIGH_REACHED | QT_STOCK_TRACE_LOG_STAT_ADD);
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL)
	{
		UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_BUYED | QT_STOCK_TRACE_LOG_STAT_ADD);
	}
}

void CStockTraceBase::ResetStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if ((pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE) || (pTraceLog->traceStep == 0))
	{
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
		pTraceLog->hisTime = 0;
		pTraceLog->realTime = 0;
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_HIGH)
	{
		UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_RAISE_BALANCED | QT_STOCK_TRACE_LOG_STAT_DEL);
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
		pTraceLog->hisTime = 0;
		pTraceLog->realTime = 0;
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
	{
		UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_HIGH_REACHED | QT_STOCK_TRACE_LOG_STAT_DEL);
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
		pTraceLog->hisTime = 0;
		pTraceLog->realTime = 0;
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL)
	{
		//UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_BUYED | QT_STOCK_TRACE_LOG_STAT_DEL);
	}
}

BOOL CStockTraceBase::IsActiveManagerAfterAddNode()
{
	return FALSE;
}

void CStockTraceBase::OnGetKLineResp(STOCK_CALC_GET_HISKLINE_RESP* pGetKLineResp)
{
	if (m_jobGetHisKine.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pGetKLineResp->respResult < 0)
	{
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobGetHisKine.hisCounts = pGetKLineResp->respResult;
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
	m_pAutoManager->ActiveManager();
}

BOOL CStockTraceBase::OnGetKLineComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_GET_HISKLINE* pGetHisKLine = (STOCK_CALC_GET_HISKLINE*)param;
	STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp = m_pAutoManager->AllocGetHisKLineRespPkt(pGetHisKLine->pTraceBase);

	if (pGetHisKLineResp == NULL)
		return FALSE;

	pGetHisKLineResp->respResult = result;
	m_pAutoManager->PostGetHisKLineRespPkt(pGetHisKLineResp);
	return TRUE;
}

void CStockTraceBase::OnUpdateTraceLogResp(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp)
{
	if (m_jobUpdateTraceLog.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pUpdateTraceLogResp->respResult < 0)
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}

	m_pAutoManager->ActiveManager();
}

BOOL CStockTraceBase::OnUpdateTraceLogComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = (STOCK_CALC_UPDATE_TRACELOG*)param;
	STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp = m_pAutoManager->AllocUpdateTraceLogRespPkt(pUpdateTraceLog->pTraceBase);

	if (pUpdateTraceLogResp == NULL)
		return FALSE;

	pUpdateTraceLogResp->respResult = result;
	m_pAutoManager->PostUpdateTraceLogRespPkt(pUpdateTraceLogResp);
	return TRUE;
}

void CStockTraceBase::OnGetCurKLineResp(STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurKLineResp)
{
	if (m_jobGetCurHisKLine.jobStep != TASK_EVENT_JOB_STEP_WAITING_RESP)
		return;

	if (pGetCurKLineResp->respResult < 0)
	{
		m_jobGetCurHisKLine.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobGetCurHisKLine.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}

	m_pAutoManager->ActiveManager();
}

BOOL CStockTraceBase::OnGetCurKLineComplete(int result, void* param, int paramLen)
{
	STOCK_CALC_GET_CUR_HISKLINE* pGetCurHisKLine = (STOCK_CALC_GET_CUR_HISKLINE*)param;
	STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp = m_pAutoManager->AllocGetCurHisKLineRespPkt(pGetCurHisKLine->pTraceBase);

	if (pGetCurHisKLineResp == NULL)
		return FALSE;

	pGetCurHisKLineResp->respResult = result;
	m_pAutoManager->PostGetCurHisKLineRespPkt(pGetCurHisKLineResp);
	return TRUE;
}

UINT CStockTraceBase::GetHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, int counts)
{
	if (m_jobGetHisKine.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return STOCK_TRACE_WORK_WAIT_RESP;

	if (m_jobGetHisKine.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		STOCK_CALC_GET_HISKLINE* pGetHisKLine = m_pAutoManager->AllocGetHisKLinePkt(this);
		if (pGetHisKLine == NULL)
			return STOCK_TRACE_WORK_BUSY;

		memcpy(pGetHisKLine->code, pTraceNode->pTraceLog->code, sizeof(pGetHisKLine->code));
		pGetHisKLine->getCnt = counts;
		pGetHisKLine->pKLineBuf = m_pHisKLine;
		m_pAutoManager->PostGetHisKLinePkt(pGetHisKLine);
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		return STOCK_TRACE_WORK_WAIT_RESP;
	}
	else if (m_jobGetHisKine.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK)
	{
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_OK;
	}
	else
	{
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_FAIL;
	}
}

UINT CStockTraceBase::UpdateTraceLog(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (m_jobUpdateTraceLog.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return STOCK_TRACE_WORK_WAIT_RESP;

	if (m_jobUpdateTraceLog.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog = m_pAutoManager->AllocUpdateTraceLogPkt(this);
		if (pUpdateTraceLog == NULL)
			return STOCK_TRACE_WORK_BUSY;

		memcpy(&pUpdateTraceLog->traceLog, pTraceNode->pTraceLog, sizeof(pUpdateTraceLog->traceLog));
	
		m_pAutoManager->PostUpdateTraceLogPkt(pUpdateTraceLog);
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		return STOCK_TRACE_WORK_WAIT_RESP;
	}
	else if (m_jobUpdateTraceLog.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK)
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_OK;
	}
	else
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_FAIL;
	}
}

UINT CStockTraceBase::GetCurHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (m_jobGetCurHisKLine.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		return STOCK_TRACE_WORK_WAIT_RESP;

	if (m_jobGetCurHisKLine.jobStep == TASK_EVENT_JOB_STEP_NONE)
	{
		STOCK_CALC_GET_CUR_HISKLINE* pGetCurHisKLine = m_pAutoManager->AllocGetCurHisKLinePkt(this);
		if (pGetCurHisKLine == NULL)
			return STOCK_TRACE_WORK_BUSY;

		pGetCurHisKLine->pCurKLine = &m_jobGetCurHisKLine.curKLine;
		memcpy(pGetCurHisKLine->code, pTraceNode->pTraceLog->code, sizeof(pGetCurHisKLine->code));
		m_pAutoManager->PostGetCurHisKLinePkt(pGetCurHisKLine);

	
		m_jobGetCurHisKLine.jobStep = TASK_EVENT_JOB_STEP_WAITING_RESP;
		return STOCK_TRACE_WORK_WAIT_RESP;
	}
	else if (m_jobGetCurHisKLine.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK)
	{
		m_jobGetCurHisKLine.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_OK;
	}
	else
	{
		m_jobGetCurHisKLine.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_FAIL;
	}
}

void CStockTraceBase::RemoveTraceNode(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	dllRemove(m_pTraceList, &pTraceNode->node);
	m_pAutoManager->AddTraceList(this, pTraceNode);
}


void CStockTraceBase::NextFromStart()
{
	m_pCurNode = DLL_FIRST(m_pTraceList);
}

void CStockTraceBase::CalcBuyEndDate(int highTime, int limits, QDate& endDate)
{
	QDateTime highDateTime = QDateTime::fromTime_t(highTime);
	QDate highDate = highDateTime.date();
	int highOffset = highDate.dayOfWeek();
	QDate highEndWeekDate = highDate.addDays(STOCK_DAYS_PER_WEEK - highOffset);

	endDate = highEndWeekDate.addDays(limits*STOCK_DAYS_PER_WEEK);
}

UINT CStockTraceBase::DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (CheckForPrepare(pTraceNode))
		return STOCK_TRACE_STEP_WORKING;
	else
		return STOCK_TRACE_STEP_END;
}

BOOL CStockTraceBase::CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	return TRUE;
}

UINT CStockTraceBase::Next(DL_NODE* pNode)
{
	if (pNode != NULL)
		m_pCurNode = pNode->next;
	return STOCK_TRACE_STEP_PRPARING;
}

UINT CStockTraceBase::DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	return STOCK_TRACE_STEP_UPDATING;
}

UINT CStockTraceBase::DoTraceUpdate(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	UINT nextStep = STOCK_TRACE_STEP_UPDATING;
	UINT result = UpdateTraceLog(pTraceNode);

	switch (result)
	{
	case STOCK_TRACE_WORK_OK:
		nextStep = STOCK_TRACE_STEP_END;
		break;

	case STOCK_TRACE_WORK_FAIL:
	case STOCK_TRACE_WORK_NONE:
		nextStep = STOCK_TRACE_STEP_END;
		break;

	case STOCK_TRACE_WORK_WAIT_RESP:
	case STOCK_TRACE_WORK_BUSY:
		break;
	}

	return nextStep;
}

BOOL CStockTraceBase::IsActiveManager()
{
	return !((m_jobGetHisKine.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP) || (m_jobUpdateTraceLog.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP)
		|| (m_jobGetCurHisKLine.jobStep == TASK_EVENT_JOB_STEP_WAITING_RESP));
}

BOOL CStockTraceBase::IsReachHigh(STOCK_CALC_TRACE_KLINE const* pStart, STOCK_CALC_TRACE_KLINE const* pCur)
{
	while (pStart != pCur)
	{
		if (pStart->fHigh > pCur->fHigh)
			return FALSE;

		pStart++;
	}

	return TRUE;
}

BOOL CStockTraceBase::IsReachHigh(STOCK_CALC_TRACE_KLINE const* pKLineEnd, int klines, STOCK_CALC_TRACE_KLINE const* pCurKLine)
{
	while (klines-- > 0)
	{
		if (pKLineEnd->fHigh > pCurKLine->fHigh)
			return FALSE;

		pKLineEnd--;
	}

	return TRUE;
}