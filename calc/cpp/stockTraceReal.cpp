/*
*stockTraceReal.cpp
*/

#include "../include/stockTraceReal.h"

CStockTraceReal::CStockTraceReal(CStockAutoManager* pAutoManager, DL_LIST* pTraceList):CStockTraceBase(pAutoManager, pTraceList)
{

}

CStockTraceReal::~CStockTraceReal()
{
	Close();
}

BOOL CStockTraceReal::Init(int hisHighCounts, float fRsiBuy, float fRsiSell, float fRsiSellWaits, float fCutLossPercent)
{
	m_iHisHighCounts = hisHighCounts;
	m_fRsiBuy = fRsiBuy;
	m_fRsiSell = fRsiSell;
	m_fRsiSellWaits = fRsiSellWaits;
	m_fCutLossPercent = fCutLossPercent;

	return CStockTraceBase::Init(hisHighCounts);
}

void CStockTraceReal::Close()
{
	CStockTraceBase::Close();
}

void CStockTraceReal::InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{

}

UINT CStockTraceReal::DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if ()
}