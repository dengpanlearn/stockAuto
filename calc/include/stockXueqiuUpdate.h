/*
*stockXiuqiuUpdate.h
*Xiuqiu网提供的https股票行情接口
*Xiuqiu网提供的https股票交易接口
*/

#ifndef  __STOCK_XUEQIU_UPDATE_H__
#define	__STOCK_XUEQIU_UPDATE_H__

#include <dp.h>
#include <global.h>
#include <stockPython.h>
#include "stockUpdateBase.h"

class CStockXiuQiuUpdate: public CStockUpdateBase
{
public:
	CStockXiuQiuUpdate();
	~CStockXiuQiuUpdate();

	BOOL Create(char const* pUpdateDir, char const* pStockListDb, char const* pStockKLineDb,  char const* pModuleDir, char const* pModule);
	void Close();

	// 接口定义
public:
	virtual BOOL UpdateLatestStockList();
	virtual BOOL UpdateLatestKLine(char const* pStockCode, int counts);
	virtual BOOL UpdateLatestKLineByTime(char const* pStockCode, time_t endTime, int counts);

private:
	char m_szModuleDir[STOCK_AUTO_SAVE_DIR_MAX_SIZE];
	CStockPython* pStockPython;
};
#endif // ! __STOCK_XUEQIU_UPDATE_H__
