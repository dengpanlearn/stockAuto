/*
*stockDataBase.h
*/

#ifndef  __STOCK_DATA_BASE_H__
#define __STOCK_DATA_BASE_H__

#include <dp.h>
#include <stockCalcDef.h>
class CStockDataBase
{
public:
	CStockDataBase() {}
	~CStockDataBase() {}

public:
	virtual BOOL StockListIsOn()=0;
	virtual int GetStockList(STOCK_CODE_NAME* pListBuf, int bufCounts)=0;

	virtual BOOL TraceLogIsOn() = 0;
	virtual int GetTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf, int bufCounts) = 0;

};
#endif // ! __STOCK_DATA_BASE_H__
