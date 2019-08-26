/*
*stockTraceReal.h
*/

#ifndef __STOCK_TRACE_REAL_H__
#define	__STOCK_TRACE_REAL_H__

#include "stockTraceBase.h"
#include "stockAutoManager.h"
class CStockTraceReal:public CStockTraceBase
{
public:
	CStockTraceReal(CStockAutoManager* pAutoManager, DL_LIST* pTraceList);
	~CStockTraceReal();

	BOOL Init(int hisHighCounts, float fRsiBuy, float fRsiSell, float fRsiSellWaits, float fCutLossPercent);
	void Close();

protected:

	virtual void InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode) = 0;
	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT Next(DL_NODE* pNode);
	virtual UINT DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);

private:
	int		m_iHisHighCounts;
	float	m_fRsiBuy;
	float	m_fRsiSell;
	float	m_fRsiSellWaits;
	float	m_fCutLossPercent;
};


#endif // !__STOCK_TRACE_REAL_H__
