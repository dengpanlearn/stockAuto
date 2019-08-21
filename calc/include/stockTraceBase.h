/*
*stockTraceBase.h
*/

#ifndef __STOCK_TRACE_BASE_H__
#define	__STOCK_TRACE_BASE_H__
#include "stockAutoManager.h"
class CStockTraceBase
{
public:
	CStockTraceBase(CStockAutoManager* pAutoManager, DL_LIST* pTraceList);
	~CStockTraceBase();

	BOOL Init(int hisKLineCounts);
	void Close();

public:
	void Trace();
	void AddTraceStock(STOCK_CALC_TRACE_NODE* pTraceNode);

public:
	void OnGetKLineResp(STOCK_CALC_GET_HISKLINE_RESP* pUpdateListResp);
	BOOL OnGetKLineComplete(int result, void* param, int paramLen);

protected:
	inline STOCK_CALC_TRACE_KLINE const* GetCurHisKLinePtr(int& counts);
	UINT GetHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, int counts);

protected:
	void TraceStock(STOCK_CALC_TRACE_NODE* pTraceNode);

	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual void Next(DL_NODE* pNode);
	virtual void DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	enum
	{
		STOCK_TRACE_PREPARE_WAIT_RESP = 0x01,
		STOCK_TRACE_PREPARE_BUSY,
		STOCK_TRACE_PREPARE_OK,
		STOCK_TRACE_PREPARE_NONE,
		STOCK_TRACE_PREPARE_FAIL,
	};
private:

	CStockAutoManager*		m_pAutoManager;
	DL_LIST*				m_pTraceList;
	STOCK_CALC_TRACE_KLINE*	m_pHisKLine;
	DL_NODE*				m_pCurNode;
	int						m_hisKLineCounts;
	STOCK_TRACE_JOB_HISKLINE_GET m_jobGetHisKine;
};

inline STOCK_CALC_TRACE_KLINE const* CStockTraceBase::GetCurHisKLinePtr(int& counts)
{
	counts = m_jobGetHisKine.hisCounts;
	return m_pHisKLine;
}
#endif // !__STOCK_TRACE_BASE_H__
