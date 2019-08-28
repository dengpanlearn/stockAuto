/*
*stockUpdateBase.h
*��Ʊ����ӿ�
*��Ʊ���׽ӿ�
*/

#ifndef  __STOCK_UPDATE_BASE_H__
#define	__STOCK_UPDATE_BASE_H__

#include <global.h>
#include <dp.h>

class CStockUpdateBase
{
public:
	CStockUpdateBase();
	~CStockUpdateBase();

	BOOL Create(char const* pUpdateDir);
	void Close();

	// �ӿڶ���
public:
	virtual BOOL UpdateLatestStockList() = 0;
	virtual BOOL UpdateLatestKLine(char const* pStockCode, int counts) = 0;
	virtual BOOL UpdateLatestKLineByTime(char const* pStockCode, time_t endTime, int counts) = 0;
	virtual BOOL GetLatestKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pCurKLine) = 0;
private:
	char m_szUpdateDir[STOCK_AUTO_SAVE_DIR_MAX_SIZE];
};
#endif // ! __STOCK_UPDATE_BASE_H__
