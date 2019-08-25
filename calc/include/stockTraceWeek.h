/*
*stockTraceWeek.h
*/

#ifndef __STOCK_TRACE_WEEK_H__
#define	__STOCK_TRACE_WEEK_H__

#include "stockTraceBase.h"

class CStockTraceWeek: public CStockTraceBase
{
public:
	CStockTraceWeek(CStockAutoManager* pAutoManager, DL_LIST* pTraceList);
	~CStockTraceWeek();

	BOOL Init(int hisKLineCounts, float fPercent, int iRaiseBalances);
	void Close();

protected:
	virtual void InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode);

	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT Next(DL_NODE* pNode);
	virtual UINT DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);

	

private:
	float	m_fPercent;
	int		m_iRaiseBalances;	
};

#endif // !__STOCK_TRACE_WEEK_H__
