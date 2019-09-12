/*
*stockDataSqlite.h
*/

#ifndef  __STOCK_DATA_SQLITE_H__
#define __STOCK_DATA_SQLITE_H__

#include <dp.h>
#include <stockCalcDef.h>
#include <global.h>
#include <qsqldatabase.h>
#include "stockDataBase.h"
class CStockDataSqlite:public CStockDataBase
{
public:
	CStockDataSqlite() {}
	~CStockDataSqlite() {}

	BOOL Init(const char* dbDir, const char* pListName, const char* pKLineName, const char* pTraceName);
	void Close();

public:
	virtual BOOL StockListIsOn();
	virtual int GetStockList(STOCK_CODE_NAME* pListBuf, int bufCounts);
	virtual BOOL TraceLogIsOn();
	virtual int GetTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf, int bufCounts);
	virtual int UpdateTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf);
	virtual int GetHisKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pHisKLineBuf, int bufCounts);

private:
	QSqlDatabase	m_listDb;
	QSqlDatabase	m_klineDb;
	QSqlDatabase	m_traceLogDb;
};

#endif // ! __STOCK_DATA_SQLITE_H__
