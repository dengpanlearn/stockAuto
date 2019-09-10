/*
*stockTraceWeek.cpp
*/

#include <qdatetime.h>
#include "../include/stockTraceWeek.h"

CStockTraceWeek::CStockTraceWeek(CStockAutoManager* pAutoManager, DL_LIST* pTraceList):CStockTraceBase(pAutoManager, pTraceList)
{

}

CStockTraceWeek::~CStockTraceWeek()
{
	Close();
}

BOOL CStockTraceWeek::Init(int hisKLineCounts, STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	m_fRaisePercent = pConfigTrace->fRaisePercent;
	m_iRaiseBalances = pConfigTrace->raiseBalances;
	m_iReachHighRanges = pConfigTrace->reachHighRanges;
	m_fRsiBuy = pConfigTrace->fRsiBuy;
	m_iRsiBuyWaits = pConfigTrace->rsiBuyWaits;
	m_fRsiSell = pConfigTrace->fRsiSell;
	return CStockTraceBase::Init(hisKLineCounts);
}

void CStockTraceWeek::Close()
{
	CStockTraceBase::Close();
}

void CStockTraceWeek::InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if (pTraceLog->traceStep == STOCK_CALC_TRACE_STEP_NONE)
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
}

BOOL CStockTraceWeek::CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;

	QDateTime hisDateTime = QDateTime::fromTime_t(pTraceLog->hisTime);
	QDateTime updateDateTime = QDateTime::fromTime_t(pTraceLog->updateTime);

	QDate hisDate = hisDateTime.date();
	QDate updateDate = updateDateTime.date();

	int offset = hisDate.dayOfWeek();
	QDate hisEndDate = hisDate.addDays(7 - offset);

	return (hisEndDate < updateDate);
}

UINT CStockTraceWeek::DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (IsJobGetHisKLineNone())
	{
		if (!CheckForPrepare(pTraceNode))
			return STOCK_TRACE_STEP_END;
	}

	UINT nextStep = STOCK_TRACE_STEP_PRPARING;
	UINT result = GetHisKLine(pTraceNode, STOCK_HIS_KLINE_MAX_COUNTS_FOR_TRACE);

	switch (result)
	{
	case STOCK_TRACE_WORK_OK:
		nextStep = STOCK_TRACE_STEP_WORKING;
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

UINT CStockTraceWeek::Next(DL_NODE* pNode)
{
	UINT result = CStockTraceBase::Next(pNode);

	STOCK_CALC_TRACE_NODE* pTraceNode = (STOCK_CALC_TRACE_NODE*)pNode;
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;


	if (pTraceLog->traceStep > CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
	{
		RemoveTraceNode(pTraceNode);
	}

	return result;
}

STOCK_CALC_TRACE_KLINE const* CStockTraceWeek::GetStartHisKLinePtr(STOCK_CALC_TRACE_KLINE const* pHisKLineEnd, int findLimits, int lastHisTime, BOOL& bLostHis)
{
	int i = 0;
	bLostHis = FALSE;
	STOCK_CALC_TRACE_KLINE const* pNextHisKLine = NULL;
	QDateTime lastHisDateTime = QDateTime::fromTime_t(lastHisTime);
	QDate lastHisDate = lastHisDateTime.date();
	int offset = 7 - lastHisDate.dayOfWeek();
	QDate lastHisEndDate = lastHisDate.addDays(offset);

	int limitsWithPrev = 2;
	findLimits += limitsWithPrev;
	for (; i < findLimits; i++, pHisKLineEnd--)
	{
		QDateTime hisDateTime = QDateTime::fromTime_t(pHisKLineEnd->timeVal);
		QDate hisDate = lastHisDateTime.date();
		QDate hisEndDate = hisDate.addDays(7- hisDate.dayOfWeek());

		if (hisEndDate < lastHisEndDate)
			break;
	}

	if (i < limitsWithPrev)
		return NULL;

	if (i >= findLimits)
		bLostHis = TRUE;
	return pHisKLineEnd + limitsWithPrev;
}

BOOL CStockTraceWeek::DoTraceWeekWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	int hisKLineCounts = 0;
	STOCK_CALC_TRACE_KLINE const* pTraceKLine = GetHisKLinePtr(hisKLineCounts);
	if (hisKLineCounts < m_iReachHighRanges)
		return FALSE;

	BOOL bLostHis = FALSE;
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	int startTraceTime = (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL) ? pTraceLog->hisTime : pTraceLog->realTime;

	STOCK_CALC_TRACE_KLINE const* pEndTraceKLine = pTraceKLine + hisKLineCounts - 1;
	STOCK_CALC_TRACE_KLINE const* pStartTraceKLine = GetStartHisKLinePtr(pEndTraceKLine, STOCK_CALC_WEEKS_LOST_MAX, startTraceTime, bLostHis);
	if (pStartTraceKLine == NULL)
		return FALSE;


	if (bLostHis && pTraceLog->traceStep != CALC_STOCK_TRADE_STEP_WAIT_SELL)
	{
		pTraceLog->raiseBalanceCheckTimes = 0;
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
	}


	do
	{
		if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
		{
			if (pStartTraceKLine->fPercent > m_fRaisePercent)
			{
				pTraceLog->raiseBalanceCheckTimes = 0;
				continue;
			}

			if (++pTraceLog->raiseBalanceCheckTimes < m_iRaiseBalances)
				continue;


			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;
		}
		else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_HIGH)
		{
			if (IsReachHigh(pTraceKLine, pStartTraceKLine))
			{
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_WAIT_BUY;
				continue;
			}

			if (pStartTraceKLine->fPercent > m_fRaisePercent)
			{
			__TRACE_INIT:
				pTraceLog->raiseBalanceCheckTimes = 0;
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
				continue;
			}

		}
		else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
		{
			if (pStartTraceKLine->fRsi7 > m_fRsiBuy)
			{
				// 历史时间不能买入, 直接重新开始流程
				goto __TRACE_INIT;
			}

			QDate rsiCheckEndForBuy;
			CalcBuyEndDate(pTraceLog->highTime, m_iRsiBuyWaits, rsiCheckEndForBuy);

			QDate curDate = QDate::currentDate();

			if (rsiCheckEndForBuy >= curDate)
				goto __TRACE_INIT;

		}
		else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL)
		{

		}
	} while (pStartTraceKLine++ <= pEndTraceKLine);


	pTraceLog->highTime = time(NULL);

	return TRUE;
}

UINT CStockTraceWeek::DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (!DoTraceWeekWork(pTraceNode))
		return STOCK_TRACE_STEP_END;
	
	return CStockTraceBase::DoTraceWork(pTraceNode);
}

UINT CStockTraceWeek::DoTraceUpdate(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	return CStockTraceBase::DoTraceUpdate(pTraceNode);
}
