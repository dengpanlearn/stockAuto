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

BOOL CStockTraceWeek::Init(int hisKLineCounts, float fPercent, int iRaiseBalances)
{
	m_fPercent = fPercent;
	m_iRaiseBalances = iRaiseBalances;
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
	if (pTraceLog->traceStep != CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE)
		return FALSE;

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

	return result;
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

BOOL CStockTraceWeek::DoTraceWeekWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	int hisKLineCounts = 0;
	STOCK_CALC_TRACE_KLINE const* pTraceKLine = GetCurHisKLinePtr(hisKLineCounts);
	if (hisKLineCounts < STOCK_HIS_KLINE_MIN_COUNTS_FOR_TRACE)
		return FALSE;

	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;

	QDateTime hisDateTime = QDateTime::fromTime_t(pTraceLog->hisTime);
	QDateTime updateDateTime = QDateTime::fromTime_t(pTraceLog->updateTime);

	QDate hisDate = hisDateTime.date();
	QDate updateDate = updateDateTime.date();

	int offset = hisDate.dayOfWeek();
	QDate hisEndDate = hisDate.addDays(7 - offset);
	long hisEndTime = QDateTime(hisEndDate).toTime_t();
	int offsetWeeks = STOCK_CALC_WEEKS_BETWEEN_SECS(pTraceLog->updateTime, hisEndTime);
	if (offsetWeeks > STOCK_CALC_WEEKS_LOST_MAX)
		offsetWeeks = STOCK_CALC_WEEKS_LOST_MAX;

	STOCK_CALC_TRACE_KLINE const* pTraceKLinePos = pTraceKLine + hisKLineCounts - offsetWeeks;

	for (int i = 0; i < offsetWeeks; i++, pTraceKLinePos++)
	{
		if (pTraceKLinePos->fPercent > m_fPercent)
		{
			pTraceLog->raiseBalanceCheckTimes = 0;
			continue;
		}

		if (++pTraceLog->raiseBalanceCheckTimes >= m_iRaiseBalances)
		{
			pTraceLog->traceStep = CALC_STOCK_TRADE_STEP_CHECK_HIGH;
			break;
		}
	}

	pTraceLog->hisTime = pTraceKLinePos->timeVal / 1000;

	return TRUE;
}

UINT CStockTraceWeek::DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	if (IsJobUpdateTraceLogNone())
	{
		if (!DoTraceWeekWork(pTraceNode))
			return STOCK_TRACE_STEP_END;
	}

	UINT nextStep = STOCK_TRACE_STEP_WORKING;
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