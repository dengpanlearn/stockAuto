/*
*stockTraceReal.cpp
*/

#include <qdatetime.h>
#include "../include/stockTraceReal.h"

CStockTraceReal::CStockTraceReal(CStockAutoManager* pAutoManager, DL_LIST* pTraceList):CStockTraceBase(pAutoManager, pTraceList)
{
	m_realTraceStep = 0;
}

CStockTraceReal::~CStockTraceReal()
{
	Close();
}

BOOL CStockTraceReal::Init(int hisHighCounts, STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	m_fRsiBuy = pConfigTrace->fRsiBuy;
	m_fRsiSell = pConfigTrace->fRsiSell;
	m_iRsiBuyWaits = pConfigTrace->rsiBuyWaits;
	m_iRsiSellWaits = pConfigTrace->rsiSellWaits;
	m_iReachHighRanges = pConfigTrace->reachHighRanges;
	m_fCutLossPercent = pConfigTrace->fCutLossPercent;
	m_fRaisePercent = pConfigTrace->fRaisePercent;
	return CStockTraceBase::Init(hisHighCounts);
}

void CStockTraceReal::Close()
{
	CStockTraceBase::Close();
}

void CStockTraceReal::InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	m_realTraceStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;
}

UINT CStockTraceReal::DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	UINT nextStep = STOCK_TRACE_STEP_PRPARING;
	switch (m_realTraceStep)
	{
	case STOCK_TRACE_REAL_PRPARE_STEP_NONE:
		if (!CheckForPrepare(pTraceNode))
		{
	_NEXT_END:
			nextStep = STOCK_TRACE_STEP_END;
			break;
		}

		m_realTraceStep = STOCK_TRACE_REAL_PRPARE_STEP_HISKLINE;	// no break
	case STOCK_TRACE_REAL_PRPARE_STEP_HISKLINE:
		if (!DoPreparerHisKLine(pTraceNode, m_realTraceStep))
			goto _NEXT_END;
		break;

	case STOCK_TRACE_REAL_PRPARE_STEP_CUR_HISKLINE:
		if (!DoPreparerCurHisKLine(pTraceNode, m_realTraceStep))
			goto _NEXT_END;

		if (m_realTraceStep == STOCK_TRACE_REAL_PRPARE_STEP_END)
		{
			m_realTraceStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;
			nextStep = STOCK_TRACE_STEP_WORKING;
		}
	
		break;
	}

	return nextStep;
}

BOOL CStockTraceReal::CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	return  (pTraceLog->traceStep > CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE);
}

BOOL CStockTraceReal::DoPreparerHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, UINT& prepareStep)
{
	prepareStep = STOCK_TRACE_REAL_PRPARE_STEP_HISKLINE;
	UINT result = GetHisKLine(pTraceNode, STOCK_HIS_KLINE_MAX_COUNTS_FOR_TRACE);

	switch (result)
	{
	case STOCK_TRACE_WORK_OK:
		prepareStep = STOCK_TRACE_REAL_PRPARE_STEP_CUR_HISKLINE;
		break;

	case STOCK_TRACE_WORK_FAIL:
	case STOCK_TRACE_WORK_NONE:
		prepareStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;
		break;

	case STOCK_TRACE_WORK_WAIT_RESP:
	case STOCK_TRACE_WORK_BUSY:
		break;
	}

	return (STOCK_TRACE_REAL_PRPARE_STEP_NONE == prepareStep);

}

BOOL CStockTraceReal::DoPreparerCurHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, UINT& prepareStep)
{
	prepareStep = STOCK_TRACE_REAL_PRPARE_STEP_CUR_HISKLINE;
	UINT result = GetCurHisKLine(pTraceNode);

	switch (result)
	{
	case STOCK_TRACE_WORK_OK:
		prepareStep = STOCK_TRACE_REAL_PRPARE_STEP_END;
		break;

	case STOCK_TRACE_WORK_FAIL:
	case STOCK_TRACE_WORK_NONE:
		prepareStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;
		break;

	case STOCK_TRACE_WORK_WAIT_RESP:
	case STOCK_TRACE_WORK_BUSY:
		break;
	}

	return (STOCK_TRACE_REAL_PRPARE_STEP_NONE == prepareStep);
}

BOOL CStockTraceReal::DoTraceRealWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_CALC_TRACE_KLINE const* pCurKLine = GetCurHisKLinePtr();
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_HIGH)
	{
		int hisKLineCounts = 0;
		STOCK_CALC_TRACE_KLINE const* pHisKLine = GetHisKLinePtr(hisKLineCounts);
		if (hisKLineCounts < m_iReachHighRanges)
			return FALSE;

		if (IsReachHigh(pHisKLine, pCurKLine))
		{
			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_WAIT_BUY;
			pTraceLog->rsiCheckTimesForBuy = 0;
			pTraceLog->highTime = time(NULL);
			pTraceLog->fHighVal = pCurKLine->fHigh;

			QDateTime highDateTime = QDateTime::fromTime_t(pTraceLog->highTime);
			QDate highDate = highDateTime.date();
			int highOffset = highDate.dayOfWeek();
			QDate highEndWeekDate = highDate.addDays(STOCK_DAYS_PER_WEEK - highOffset);

			m_rsiCheckEndDateForBuy = highEndWeekDate.addDays(m_iRsiBuyWaits*STOCK_DAYS_PER_WEEK);
		}
		else
		{
			if (!CALC_IN_DEADZONE(pCurKLine->fPercent, m_fRaisePercent))
			{
			__TRACE_INIT:
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
				pTraceLog->raiseBalanceCheckTimes = 0;
			}
		}
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
	{
		if (pCurKLine->fRsi7 > m_fRsiBuy)
		{
			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_WAIT_SELL;
			pTraceLog->buyTime = time(NULL);
			pTraceLog->fBuyVal = pCurKLine->fClose;
			pTraceLog->rsiCheckTimesForSell = 0;
		}
		else
		{
			QDate curDate = QDate::currentDate();
			if (curDate >= m_rsiCheckEndDateForBuy)
			{
				goto __TRACE_INIT;
			}
		}
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL)
	{
		if (pCurKLine->fClose < pCurKLine->fMa10)
		{
		_TRACE_SELL:
			pTraceLog->sellTime = time(NULL);
			pTraceLog->fSellVal = pCurKLine->fClose;

			goto __TRACE_INIT;
		}
		
		if ((pTraceLog->fBuyVal - pCurKLine->fClose) / pTraceLog->fBuyVal >
			m_fCutLossPercent)
			goto _TRACE_SELL;

		for (int i = 0; i < m_iRsiSellWaits-1; i++)
		{

		}
	}
}

UINT CStockTraceReal::DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{

}