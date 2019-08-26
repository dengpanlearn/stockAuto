/*
*stockTraceBase.cpp
*/

#include "../include/stockTraceBase.h"

CStockTraceBase::CStockTraceBase(CStockAutoManager* pAutoManager, DL_LIST* pTraceList)
{
	m_pAutoManager = pAutoManager;
	m_pTraceList = pTraceList;

	memset(&m_jobGetHisKine, 0, sizeof(m_jobGetHisKine));
	memset(&m_jobUpdateTraceLog, 0, sizeof(m_jobUpdateTraceLog));
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
	else
	{
		m_workStep = Next(m_pCurNode);
	}
	
	
}

void CStockTraceBase::AddTraceStock(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	dllAdd(m_pTraceList, &pTraceNode->node);
}

void CStockTraceBase::TraceStock(STOCK_CALC_TRACE_NODE* pTraceNode)
{

}

void CStockTraceBase::OnGetKLineResp(STOCK_CALC_GET_HISKLINE_RESP* pGetKLineResp)
{
	if (pGetKLineResp->respResult < 0)
	{
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobGetHisKine.hisCounts = pGetKLineResp->respResult;
		m_jobGetHisKine.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
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
	if (pUpdateTraceLogResp->respResult < 0)
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_FAIL;
	}
	else
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_COMPLETED_OK;
	}
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
	}
	else if (m_jobUpdateTraceLog.jobStep == TASK_EVENT_JOB_STEP_COMPLETED_OK)
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_OK;
	}
	else
	{
		m_jobUpdateTraceLog.jobStep = TASK_EVENT_JOB_STEP_NONE;
		return STOCK_TRACE_WORK_OK;
	}
}

void CStockTraceBase::RemoveTraceNode(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	dllRemove(m_pTraceList, &pTraceNode->node);
	m_pAutoManager->AddTraceList(this, pTraceNode);
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
	return STOCK_TRACE_STEP_END;
}