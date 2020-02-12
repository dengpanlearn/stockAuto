/*
*stockTraceWeek.cpp
*/

#include <qdatetime.h>
#include <qtStockTraceDef.h>
#include "../include/stockConfigDef.h"
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
	m_fHighLossPercent = pConfigTrace->fHighLostPercent;
	m_iRaiseBalanceWaits = pConfigTrace->raiseBalanceWaits;
	m_fVolumePercentBuy = pConfigTrace->fVolumePercentBuy;
	m_fRsiBuy = pConfigTrace->fRsiBuy;
	m_iRsiBuyWaits = pConfigTrace->rsiBuyWaits;
	m_iVolumeBuyWaits = pConfigTrace->volumeBuyWaits;
	m_fPrevVolumePercentLost = pConfigTrace->fPrevVolumePercentLost;
	m_fRsiSell = pConfigTrace->fRsiSell;
	return CStockTraceBase::Init(hisKLineCounts);
}

void CStockTraceWeek::Close()
{
	CStockTraceBase::Close();
}

void CStockTraceWeek::UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	m_iVolumeBuyWaits = pConfigTrace->volumeBuyWaits;
	m_fPrevVolumePercentLost = pConfigTrace->fPrevVolumePercentLost;
	m_fRaisePercent = pConfigTrace->fRaisePercent;
	m_iRaiseBalances = pConfigTrace->raiseBalances;
	m_iReachHighRanges = pConfigTrace->reachHighRanges;
	m_fVolumePercentBuy = pConfigTrace->fVolumePercentBuy;
	m_fRsiBuy = pConfigTrace->fRsiBuy;
	m_iRsiBuyWaits = pConfigTrace->rsiBuyWaits;
	m_fRsiSell = pConfigTrace->fRsiSell;
}

void CStockTraceWeek::InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	if (pTraceLog->traceStep == STOCK_CALC_TRACE_STEP_NONE)
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;

	CStockTraceBase::InitStockTrace(pTraceNode);
}

void CStockTraceWeek::ResetStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	CStockTraceBase::ResetStockTrace(pTraceNode);
}

BOOL CStockTraceWeek::IsActiveManagerAfterAddNode()
{
	return TRUE;
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

	QDateTime lastHisDateTime = QDateTime::fromTime_t(lastHisTime);
	QDate lastHisDate = lastHisDateTime.date();
	int offset = 7 - lastHisDate.dayOfWeek();
	QDate lastHisEndDate = lastHisDate.addDays(offset);

	int limitsWithPrev = 1;
	findLimits += limitsWithPrev;
	for (; i < findLimits; i++, pHisKLineEnd--)
	{
		QDateTime hisDateTime = QDateTime::fromTime_t(pHisKLineEnd->timeVal);
		QDate hisDate = hisDateTime.date();
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
	STOCK_CALC_TRACE_KLINE const* pHighKLinePtr = NULL;
	STOCK_CALC_TRACE_KLINE const* pTraceKLine = GetHisKLinePtr(hisKLineCounts);
	if (hisKLineCounts < m_iReachHighRanges)
		return FALSE;

	BOOL bLostHis = FALSE;
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	int startTraceTime = (pTraceLog->traceStep <= CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)? pTraceLog->hisTime: pTraceLog->realTime;

	STOCK_CALC_TRACE_KLINE const* pEndTraceKLine = pTraceKLine + hisKLineCounts - 1;
	STOCK_CALC_TRACE_KLINE const* pStartTraceKLine = GetStartHisKLinePtr(pEndTraceKLine, STOCK_CALC_WEEKS_LOST_MAX, startTraceTime, bLostHis);
	if (pStartTraceKLine == NULL)
		return FALSE;

	if (pStartTraceKLine == pEndTraceKLine)
	{
		if (pTraceLog->traceStep <= CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
			return FALSE;
	}


	if (bLostHis && pTraceLog->traceStep != CALC_STOCK_TRADE_STEP_WAIT_SELL)
	{
		if (pTraceLog->traceStep != CALC_STOCK_TRADE_STEP_CHECK_HIGH)
			UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code,  QT_STOCK_TRACE_LOG_STAT_DEL);

		pTraceLog->raiseBalanceCheckTimes = 0;
		pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;
	}

	if (pTraceLog->traceStep <= CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
		pStartTraceKLine++;

	//if (strcmp(pTraceNode->pTraceLog->code, "SZ300416") == 0)
		//printf("debug\n");
	do
	{
	_AGAIN:
		if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_HIGH)
		{
			STOCK_CALC_TRACE_KLINE const* pCalcHighStart = pStartTraceKLine - m_iReachHighRanges;
			if (IsReachHigh((pCalcHighStart <pTraceKLine) ? pTraceKLine : pCalcHighStart, pStartTraceKLine))
			{
				pTraceLog->highTime = pStartTraceKLine->timeVal;
				pTraceLog->fHighVal = pStartTraceKLine->fHigh;
				pHighKLinePtr = pStartTraceKLine;
			
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE;
				UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_HIGH_REACHED | QT_STOCK_TRACE_LOG_STAT_ADD);
			}
		}
		else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
		{
			float fHighLossPercent = (pTraceLog->fHighVal - pStartTraceKLine->fClose) * 100 / pTraceLog->fHighVal;
		
			if (pHighKLinePtr == NULL)
			{
				pHighKLinePtr = GetHighHisKLinePtr(pStartTraceKLine - 1, STOCK_CALC_WEEKS_LOST_MAX, pTraceLog->highTime);
			}
			// �򵥵Ĵ���
			if ((fHighLossPercent > m_fHighLossPercent) ||  (pHighKLinePtr == NULL) || (pStartTraceKLine > (pHighKLinePtr+m_iRaiseBalanceWaits))
				|| (pStartTraceKLine->fClose < pStartTraceKLine->fMa10))
			{
			__TRACE_INIT:
				pTraceLog->raiseBalanceCheckTimes = 0;
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;
				if (pHighKLinePtr != NULL)
					pStartTraceKLine = ++pHighKLinePtr;

				pHighKLinePtr = NULL;
				UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_DEL);

				goto _AGAIN;
			}

			if (IsRaiseBalance(pStartTraceKLine, pHighKLinePtr, m_fRaisePercent, m_iRaiseBalances))
			{
				pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_WAIT_BUY;
				UpdateStockTraceStat(pTraceNode->stockIdx, pTraceLog->code, QT_STOCK_TRACE_LOG_STAT_RAISE_BALANCED | QT_STOCK_TRACE_LOG_STAT_MODIFY);
			}
		}
		else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUY)
		{
			STOCK_CALC_TRACE_KLINE const* pLastKLine = pStartTraceKLine-1;
			STOCK_CALC_TRACE_KLINE const* pPrevLastKLine = pLastKLine - 1;
			STOCK_CALC_TRACE_KLINE const* pPrevLastLastKLine = pPrevLastKLine - 1;
			float fVolumePercent = 0;
			float fPreVolumePercent = 0;
			float fPreLastVolumePercent = 0;
			if (pStartTraceKLine->volume > pLastKLine->volume)
				fVolumePercent = (float)(pStartTraceKLine->volume - pLastKLine->volume) * 100 / pLastKLine->volume;

			if (pPrevLastKLine->volume > pLastKLine->volume)
				fPreVolumePercent = (float)(pPrevLastKLine->volume - pLastKLine->volume) * 100 / pLastKLine->volume;

			if (pPrevLastLastKLine->volume > pLastKLine->volume)
				fPreLastVolumePercent = (float)(pPrevLastLastKLine->volume - pLastKLine->volume) * 100 / pLastKLine->volume;

			float fHighLossPercent = (pTraceLog->fHighVal - pStartTraceKLine->fClose) * 100 / pTraceLog->fHighVal;
;
			if ((fHighLossPercent > m_fHighLossPercent) || ((fVolumePercent > m_fVolumePercentBuy) && ((fPreLastVolumePercent > m_fPrevVolumePercentLost) || 
				(fPreVolumePercent > m_fPrevVolumePercentLost))) ||
				!IsInHisKLineRange(pStartTraceKLine, m_iVolumeBuyWaits, pTraceLog->highTime))
			{
				if (pHighKLinePtr == NULL)
					pHighKLinePtr = GetHighHisKLinePtr(pStartTraceKLine - 1, STOCK_CALC_WEEKS_LOST_MAX*2, pTraceLog->highTime);
				goto __TRACE_INIT;
			}

		}
		else if (pTraceLog->traceStep == CALC_STOCK_TRADE_STEP_WAIT_SELL)
		{

		}
	} while (pStartTraceKLine++ < pEndTraceKLine);

	__TRACE_END:
	pTraceLog->hisTime = pTraceLog->updateTime;
	if (pTraceLog->traceStep > CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
		pTraceLog->realTime = pTraceLog->updateTime;

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

BOOL CStockTraceWeek::IsActiveManager()
{
	return CStockTraceBase::IsActiveManager();
}
