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

	BOOL Init(int hisKLineCounts, STOCKAUTO_CONFIG_TRACE const* pConfigTrace);
	void Close();
	virtual void UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace);

protected:
	static STOCK_CALC_TRACE_KLINE const* GetStartHisKLinePtr(STOCK_CALC_TRACE_KLINE const* pHisKLineEnd, int findLimits, int lastHisTime, BOOL& bLostHis);
	virtual void InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual void ResetStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL IsActiveManagerAfterAddNode();
	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT Next(DL_NODE* pNode);
	virtual UINT DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT DoTraceUpdate(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL IsActiveManager();

private:
	BOOL DoTraceWeekWork(STOCK_CALC_TRACE_NODE* pTraceNode);

private:
	float	m_fRaisePercent;
	int		m_iRaiseBalances;	
	int		m_iReachHighRanges;
	float	m_fHighLossPercent;
	int		m_iRaiseBalanceWaits;
	float	m_fPrevVolumePercentLost;
	float	m_fVolumePercentBuy;
	float	m_fRsiBuy;
	int		m_iVolumeBuyWaits;
	int		m_iRsiBuyWaits;
	float	m_fRsiSell;
};

#endif // !__STOCK_TRACE_WEEK_H__
