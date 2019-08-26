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
	void OnGetKLineResp(STOCK_CALC_GET_HISKLINE_RESP* pGetKLineResp);
	BOOL OnGetKLineComplete(int result, void* param, int paramLen);

	void OnUpdateTraceLogResp(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp);
	BOOL OnUpdateTraceLogComplete(int result, void* param, int paramLen);

protected:
	inline STOCK_CALC_TRACE_KLINE const* GetCurHisKLinePtr(int& counts);

	UINT GetHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, int counts);
	UINT UpdateTraceLog(STOCK_CALC_TRACE_NODE* pTraceNode);


	inline BOOL IsJobGetHisKLineNone();
	inline BOOL IsJobUpdateTraceLogNone();

	void RemoveTraceNode(STOCK_CALC_TRACE_NODE* pTraceNode);

protected:
	void TraceStock(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual void InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode) = 0;
	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT Next(DL_NODE* pNode);
	virtual UINT DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	enum
	{
		STOCK_TRACE_WORK_WAIT_RESP = 0x01,
		STOCK_TRACE_WORK_BUSY,
		STOCK_TRACE_WORK_OK,
		STOCK_TRACE_WORK_NONE,
		STOCK_TRACE_WORK_FAIL,

		STOCK_TRACE_STEP_PRPARING = 0x01,
		STOCK_TRACE_STEP_WORKING,
		STOCK_TRACE_STEP_END,
	};
private:

	CStockAutoManager*		m_pAutoManager;
	DL_LIST*				m_pTraceList;
	STOCK_CALC_TRACE_KLINE*	m_pHisKLine;
	DL_NODE*				m_pCurNode;
	int						m_hisKLineCounts;
	STOCK_TRACE_JOB_HISKLINE_GET m_jobGetHisKine;
	STOCK_TRACE_JOB_TRACELOG_UPDATE m_jobUpdateTraceLog;
	UINT					m_workStep;
};

inline STOCK_CALC_TRACE_KLINE const* CStockTraceBase::GetCurHisKLinePtr(int& counts)
{
	counts = m_jobGetHisKine.hisCounts;
	return m_pHisKLine;
}

inline BOOL CStockTraceBase::IsJobGetHisKLineNone()
{
	return m_jobGetHisKine.jobStep == TASK_EVENT_JOB_STEP_NONE;
}

inline BOOL CStockTraceBase::IsJobUpdateTraceLogNone()
{
	return m_jobUpdateTraceLog.jobStep == TASK_EVENT_JOB_STEP_NONE;
}
#endif // !__STOCK_TRACE_BASE_H__
