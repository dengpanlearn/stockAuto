/*
*stockTraceBase.h
*/

#ifndef __STOCK_TRACE_BASE_H__
#define	__STOCK_TRACE_BASE_H__
#include "stockAutoManager.h"
#include <qdatetime.h>
class CStockTraceBase
{
public:
	CStockTraceBase(CStockAutoManager* pAutoManager, DL_LIST* pTraceList);
	~CStockTraceBase();

	BOOL Init(int hisKLineCounts);
	void Close();

	virtual void UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace) {}
	virtual void ResetTrace();
public:
	void Trace();
	void AddTraceStock(STOCK_CALC_TRACE_NODE* pTraceNode);

public:
	void OnGetKLineResp(STOCK_CALC_GET_HISKLINE_RESP* pGetKLineResp);
	BOOL OnGetKLineComplete(int result, void* param, int paramLen);

	void OnUpdateTraceLogResp(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp);
	BOOL OnUpdateTraceLogComplete(int result, void* param, int paramLen);

	void OnGetCurKLineResp(STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurKLineResp);
	BOOL OnGetCurKLineComplete(int result, void* param, int paramLen);

	void OnInsertTraceRecordResp(STOCK_CALC_INSERT_TRACERECORD_RESP* pInsertTraceReocrdResp);
	BOOL OnInsertTraceRecordComplete(int result, void* param, int paramLen);

protected:
	inline STOCK_CALC_TRACE_KLINE const* GetHisKLinePtr(int& counts);
	inline STOCK_CALC_TRACE_KLINE const* GetCurHisKLinePtr();

	UINT GetHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode, int counts);
	UINT UpdateTraceLog(STOCK_CALC_TRACE_NODE* pTraceNode);
	UINT GetCurHisKLine(STOCK_CALC_TRACE_NODE* pTraceNode);
	UINT InsertTraceRecord(STOCK_CALC_TRACE_NODE* pTraceNode);
	inline BOOL IsJobGetHisKLineNone();
	inline BOOL IsJobUpdateTraceLogNone();

	void RemoveTraceNode(STOCK_CALC_TRACE_NODE* pTraceNode);
	void NextFromStart();
	void CalcBuyEndDate(int highTime, int limits, QDate& endDate);
protected:
	static BOOL IsReachHigh(STOCK_CALC_TRACE_KLINE const* pStart, STOCK_CALC_TRACE_KLINE const* pCur);
	static BOOL IsReachHigh(STOCK_CALC_TRACE_KLINE const* pKLineEnd, int klines, STOCK_CALC_TRACE_KLINE const* pCurKLine);
	static BOOL IsRaiseBalance(STOCK_CALC_TRACE_KLINE const* pStart, STOCK_CALC_TRACE_KLINE const* pHighPtr, float fRaiseLimits, int rangesLimit);
	static BOOL IsEndOfCurTraceWeek();
	static STOCK_CALC_TRACE_KLINE const* GetHighHisKLinePtr(STOCK_CALC_TRACE_KLINE const* pHisKLineEnd, int findLimits, long time);
	void TraceStock(STOCK_CALC_TRACE_NODE* pTraceNode);
	void UpdateStockTraceStat(int stockIdx, char const* pCode, UINT stat);

	virtual void InitStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual void ResetStockTrace(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL IsActiveManagerAfterAddNode();
	virtual UINT DoPrepareWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL CheckForPrepare(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT Next(DL_NODE* pNode);
	virtual UINT DoTraceWork(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual UINT DoTraceUpdate(STOCK_CALC_TRACE_NODE* pTraceNode);
	virtual BOOL IsActiveManager();
	enum
	{
		STOCK_TRACE_WORK_WAIT_RESP = 0x01,
		STOCK_TRACE_WORK_BUSY,
		STOCK_TRACE_WORK_OK,
		STOCK_TRACE_WORK_NONE,
		STOCK_TRACE_WORK_FAIL,

		STOCK_TRACE_STEP_PRPARING = 0x01,
		STOCK_TRACE_STEP_WORKING,
		STOCK_TRACE_STEP_UPDATING,
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
	STOCK_TRACE_JOB_CURHISKLINE_GET m_jobGetCurHisKLine;
	STOCK_TRACE_JOB_TRACERECORD_INSERT m_jobInsertTraceRecord;
	UINT					m_workStep;
};

inline STOCK_CALC_TRACE_KLINE const* CStockTraceBase::GetHisKLinePtr(int& counts)
{
	counts = m_jobGetHisKine.hisCounts;
	return m_pHisKLine;
}

inline STOCK_CALC_TRACE_KLINE const* CStockTraceBase::GetCurHisKLinePtr()
{
	return &m_jobGetCurHisKLine.curKLine;
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
