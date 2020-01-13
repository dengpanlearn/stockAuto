/*
*stockDataBase.h
*/

#ifndef  __STOCK_DATA_BASE_H__
#define __STOCK_DATA_BASE_H__

#include <dp.h>
#include <stockCalcDef.h>

struct QT_STOCK_TRACERECORD_INFO;

class CStockDataBase
{
public:
	CStockDataBase() {}
	virtual ~CStockDataBase() {}

public:
	virtual BOOL StockListIsOn()=0;
	virtual int GetStockList(STOCK_CODE_NAME* pListBuf, int bufCounts)=0;

	virtual BOOL TraceLogIsOn() = 0;
	virtual BOOL TraceRecordIsOn() = 0;
	virtual int GetTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf, int bufCounts) = 0;
	virtual int ClearTraceLogHisTime() = 0;
	virtual int UpdateTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf) = 0;
	virtual int GetHisKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pHisKLineBuf, int bufCounts)=0;
	virtual int GetTraceLog(char const* pStockCode, STOCK_MANAGER_TRACE_LOG* pTraceLogBuf)=0;
	virtual int InsertTraceRecord(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf) = 0;
	virtual int GetTraceRecord(QT_STOCK_TRACERECORD_INFO* pTraceRecord, int startTime, int endTime, int counts, int offset)=0;
};
#endif // ! __STOCK_DATA_BASE_H__
