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

	BOOL Init(int hisHighCounts, STOCKAUTO_CONFIG_TRACE const* pConfigTrace);
	void Close();

	virtual void UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace);
	virtual void ResetTrace();
protected:
	static BOOL IsHisKLineRsiContinueLow(STOCK_CALC_TRACE_KLINE const* pHisKLineEnd, int times, float fRsiLimit);

	virtual void InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT Next(DL_NODE* pNode);
	virtual UINT DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT DoTraceUpdate(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL IsActiveManager();

private:
	BOOL DoPreparerHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, UINT& prepareStep);
	BOOL DoPreparerCurHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, UINT& prepareStep);
	BOOL DoPreparerEnd(STOCK_CALC_TRACE_NODE* pTraceNode, UINT& prepareStep);
	BOOL DoTraceRealWork(STOCK_CALC_TRACE_NODE* pTraceNode);

private:
	enum
	{
		STOCK_TRACE_REAL_PRPARE_STEP_NONE = 0x00,
		STOCK_TRACE_REAL_PRPARE_STEP_HISKLINE,
		STOCK_TRACE_REAL_PRPARE_STEP_CUR_HISKLINE,
		STOCK_TRACE_REAL_PRPARE_STEP_END,
	};

	UINT	m_realTraceStep;
	float	m_fRaisePercent;
	int		m_iReachHighRanges;
	float	m_fRsiBuy;
	int		m_iRsiBuyWaits;
	float	m_fRsiSell;
	int		m_iRsiSellWaits;
	float	m_fCutLossPercent;
	QDate	m_rsiCheckEndDateForBuy;
};


#endif // !__STOCK_TRACE_REAL_H__
