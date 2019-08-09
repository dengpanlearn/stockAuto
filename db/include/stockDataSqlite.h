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

	BOOL Init(const char* dbDir, const char* pListName, const char* pKLineName);
	void Close();

protected:
	virtual BOOL StockListIsOn();
	virtual int GetStockList(STOCK_CODE_NAME* pListBuf, int bufCounts);

private:
	QSqlDatabase	m_listDb;
	QSqlDatabase	m_klineDb;
};

#endif // ! __STOCK_DATA_SQLITE_H__
