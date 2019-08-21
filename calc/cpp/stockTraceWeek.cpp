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

UINT CStockTraceWeek::DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	STOCK_MANAGER_TRACE_LOG* pTraceLog = pTraceNode->pTraceLog;
	QDateTime hisDateTime = QDateTime::fromTime_t(pTraceLog->hisTime);
	QDateTime updateDateTime = QDateTime::fromTime_t(pTraceLog->updateTime);

	QDate hisDate = hisDateTime.date();
	QDate updateDate = updateDateTime.date();

	int offset = hisDate.dayOfWeek();
	QDate hisEndDate = hisDate.addDays(7- offset);

	if (hisEndDate >= updateDate)
		return STOCK_TRACE_PREPARE_NONE;

	return GetHisKLine(pTraceNode, STOCK_HIS_KLINE_MAX_COUNTS_FOR_TRACE);
}

void CStockTraceWeek::Next(DL_NODE* pNode)
{

}


void CStockTraceWeek::DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode)
{
	int hisKLineCounts = 0;
	STOCK_CALC_TRACE_KLINE const* pTraceKLine = GetCurHisKLinePtr(hisKLineCounts);
	if (hisKLineCounts < STOCK_HIS_KLINE_MIN_COUNTS_FOR_TRACE)
		return;

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

	STOCK_CALC_TRACE_KLINE const* pTraceKLinePos = pTraceKLine + hisKLineCounts - STOCK_CALC_WEEKS_LOST_MAX;

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
			pTraceLog->hisTime = pTraceKLinePos->timeVal;
			break;
		}
		
	}
}