/*
*stockTraceReal.cpp
*/

#include <qdatetime.h>
#include <qtStockTraceDef.h>
#include "../include/stockConfigDef.h"
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
	m_fHighLossPercent = pConfigTrace->fHighLostPercent;
	m_fVolumePercentBuy = pConfigTrace->fVolumePercentBuy;
	m_fCutLossPercent = pConfigTrace->fCutLossPercent;
	m_fRaisePercent = pConfigTrace->fRaisePercent;
	m_fCutLossAfterTop = pConfigTrace->fCutLossAfterTop;
	return CStockTraceBase::Init(hisHighCounts);
}

void CStockTraceReal::Close()
{
	CStockTraceBase::Close();
}

void CStockTraceReal::UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	m_fRsiBuy = pConfigTrace->fRsiBuy;
	m_fRsiSell = pConfigTrace->fRsiSell;
	m_iRsiBuyWaits = pConfigTrace->rsiBuyWaits;
	m_iRsiSellWaits = pConfigTrace->rsiSellWaits;
	m_iReachHighRanges = pConfigTrace->reachHighRanges;
	m_fHighLossPercent = pConfigTrace->fHighLostPercent;
	m_fVolumePercentBuy = pConfigTrace->fVolumePercentBuy;
	m_fCutLossPercent = pConfigTrace->fCutLossPercent;
	m_fRaisePercent = pConfigTrace->fRaisePercent;
	m_fCutLossAfterTop = pConfigTrace->fCutLossAfterTop;
}

BOOL CStockTraceReal::IsHisKLineRsiContinueLow(STOCK_CALC_TRACE_KLINE const* pHisKLineEnd, int times, float fRsiLimit)
{
	for (int i = 0; i < times; pHisKLineEnd--)
	{
		if (pHisKLineEnd->fRsi7 > fRsiLimit)
			return FALSE;
	}

	return TRUE;
}

void CStockTraceReal::InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	m_realTraceStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;
	CStockTraceBase::InitStockTrace(pTraceNode);
}

void CStockTraceReal::ResetStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	CStockTraceBase::ResetStockTrace(pTraceNode);
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if (pTraceLog->traceStep <= CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
		RemoveTraceNode(pTraceNode);
}

BOOL CStockTraceReal::IsActiveManagerAfterAddNode()
{
	return FALSE;
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
			m_realTraceStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;
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
		break;

	case STOCK_TRACE_REAL_PRPARE_STEP_END:
		if (!DoPreparerEnd(pTraceNode, m_realTraceStep))
			nextStep = STOCK_TRACE_STEP_END;
		else 
			nextStep = STOCK_TRACE_STEP_WORKING;
		break;
	}

	return nextStep;
}

UINT CStockTraceReal::Next(DL_NODE* pNode)
{
	UINT result = CStockTraceBase::Next(pNode);

	STOCK_CALC_TRACE_NODE* pTraceNode = (STOCK_CALC_TRACE_NODE*)pNode;
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;


	if (pTraceLog->traceStep <= CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
	{
		RemoveTraceNode(pTraceNode);
	}

	if (pNode->next == NULL)
	{
		NextFromStart();
	}

	return result;
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

	return (STOCK_TRACE_REAL_PRPARE_STEP_NONE != prepareStep);

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

	return (STOCK_TRACE_REAL_PRPARE_STEP_NONE != prepareStep);
}

BOOL CStockTraceReal::DoPreparerEnd(STOCK_CALC_TRACE_NODE* pTraceNode, UINT& prepareStep)
{
	if (prepareStep == STOCK_TRACE_REAL_PRPARE_STEP_END)
	{
		STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
		if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
		{
			CalcBuyEndDate(pTraceLog->highTime, m_iRsiBuyWaits, m_rsiCheckEndDateForBuy);
		}
	}

	m_realTraceStep = STOCK_TRACE_REAL_PRPARE_STEP_NONE;

	return CheckRealWorkValid(pTraceNode);
}

BOOL CStockTraceReal::CheckRealWorkValid(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	QDate curDate = QDate::currentDate();
	STOCK_CALC_TRACE_KLINE const* pCurKLine = GetCurHisKLinePtr();
	QDate curHisDate = QDateTime::fromTime_t(pCurKLine->timeVal).date();
	return (curHisDate >= curDate);
}

BOOL CStockTraceReal::DoTraceRealWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	int hisKLineCounts;
	STOCK_CALC_TRACE_KLINE const* pCurKLine = GetCurHisKLinePtr();
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	STOCK_CALC_TRACE_KLINE const* pHisKLine = GetHisKLinePtr(hisKLineCounts);
	STOCK_CALC_TRACE_KLINE const* pHisKLineEnd = pHisKLine + hisKLineCounts - 1;


	if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
	{
		float fVolumePercent = (float)(pCurKLine->volume - pHisKLineEnd->volume)*100 / pHisKLineEnd->volume;
		float fHighLossPercent = (pTraceLog->fHighVal - pCurKLine->fClose) * 100 / pTraceLog->fHighVal;

		if (fHighLossPercent > m_fHighLossPercent )
		{
		__TRACE_INIT:
			pTraceLog->hisTime = pTraceLog->highTime;		// 从创新高的时间开始计算
			pTraceLog->raiseBalanceCheckTimes = 0;
			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;
			UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_DEL);
		}
		else if (fVolumePercent > m_fVolumePercentBuy)
		{
			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_WAIT_SELL;
			pTraceLog->buyTime = time(NULL);
			pTraceLog->fBuyVal = pCurKLine->fClose;
			pTraceLog->fTopVal = pCurKLine->fClose;
			pTraceLog->topTime = pTraceLog->buyTime;
			UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_VOLUME_REACHED | QT_STOCK_TRACE_LOG_STAT_MODIFY);
			// 没有买入接口，
			UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_BUYING | QT_STOCK_TRACE_LOG_STAT_MODIFY);
			UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_BUYED | QT_STOCK_TRACE_LOG_STAT_MODIFY);
		}
	}
	else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL)
	{
		if (pCurKLine->fClose < pCurKLine->fMa10)
		{
			goto _TRACE_SELL;
		}
		
		if ((pTraceLog->fBuyVal - pCurKLine->fClose)*100 / pTraceLog->fBuyVal >
			m_fCutLossPercent)
			goto _TRACE_SELL;

		if (IsHisKLineRsiContinueLow(pHisKLineEnd, m_iRsiSellWaits - 1, m_fRsiSell))
		{
			if (pCurKLine->fRsi7 < m_fRsiSell)
				goto _TRACE_SELL;
		}

		
		if (pCurKLine->fClose > pTraceLog->fTopVal)
		{
			pTraceLog->fTopVal = pCurKLine->fClose;
			pTraceLog->topTime = time(NULL);
		}
		else
		{
			float fPercentTop = (pTraceLog->fTopVal - pCurKLine->fClose) * 100 / pTraceLog->fTopVal;
			if (fPercentTop > m_fCutLossAfterTop)
			{
	_TRACE_SELL:
				pTraceLog->sellTime = time(NULL);
				pTraceLog->fSellVal = pCurKLine->fClose;
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_WAIT_SELLING;
			}
		}
	}

	pTraceLog->realTime = time(NULL);
	return TRUE;
}

UINT CStockTraceReal::DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (!DoTraceRealWork(pTraceNode))
		return STOCK_TRACE_STEP_END;

	UINT nextStep = STOCK_TRACE_STEP_UPDATING;
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELLING)
	{
		UINT insertStep = InsertTraceRecord(pTraceNode);
		switch (insertStep)
		{
		case STOCK_TRACE_WORK_BUSY:
		case STOCK_TRACE_WORK_WAIT_RESP:
			nextStep = STOCK_TRACE_STEP_WORKING;
			break;

		case STOCK_TRACE_WORK_FAIL:
		case STOCK_TRACE_WORK_OK:
		default:
			pTraceLog->hisTime = pTraceLog->highTime;		// 从创新高的时间开始计算
			pTraceLog->raiseBalanceCheckTimes = 0;
			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;
			UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_DEL);
			nextStep = STOCK_TRACE_STEP_UPDATING;
			break;
		}
	}

	return nextStep;
}

UINT CStockTraceReal::DoTraceUpdate(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	return CStockTraceBase::DoTraceUpdate(pTraceNode);
}

BOOL CStockTraceReal::IsActiveManager()
{
	return FALSE;
}
