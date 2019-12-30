/*
*stockAutoManager.h
*/

#ifndef __STOCK_AUTO_MANAGER_H__
#define __STOCK_AUTO_MANAGER_H__

#include <multiEventsTask.h>
#include "stockCalcDef.h"
#include "stockDataTask.h"
#include "stockUpdateTask.h"

#define	TRACE_MANAGER_TASK_EVENT_NAME_PREFIX	_T("event_traceman_")

class CStockAutoWindow;
struct QT_STOCK_TRACEINFO_QUERY_PARAM;
class CStockAutoManager :public CMultiEventsTask
{
public:
	CStockAutoManager();
	virtual ~CStockAutoManager();

	BOOL Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize, CStockAutoWindow* pAutoWindow);
	void Close();

public:
	inline CMultiEventsTask* GetTaskData();
	inline CMultiEventsTask* GetTaskUpdate();
	void ActiveManager();
	void InactiveManager();

	BOOL UpdateStockTraceStat(int stockIdx, char const* pStockCode, UINT updateStat);
	const char* GetStockNameByCode(const char* pCode);

	BOOL UpdateCalcTraceConfig(STOCKAUTO_CONFIG_TRACE const* pConfigTrace);

protected:
	void InitConfig();
	void OnEventManager();


	virtual int GetTaskEvent(DP_EVENT_ID* pEventsBuf, int maxCount)const;
	virtual int GetTaskEventCount()const;
	virtual void OnSubEventActive(UINT evnetNum);

	virtual void OnExit();
	virtual UINT PreActive(UINT timeout);
	virtual BOOL CheckSelf();
	virtual int OnEventActive(UINT cmd, void* param, int paramLen);
	virtual BOOL OnEventComplete(UINT cmd, int result, void* param, int paramLen);
	virtual void OnTimeout();

private:
	void InitStockTraceByLog(STOCK_MANAGER_JOB_TRACELOG_LOAD* pJobTraceLog, STOCK_MANAGER_JOB_LIST* pJobList);
	void InitStockTraceCalc(STOCK_MANAGER_TRACE_LOG* pTraceLog, int logCounts);

public:
	STOCK_CALC_GET_HISKLINE* AllocGetHisKLinePkt(CStockTraceBase* pTraceBase);
	void PostGetHisKLinePkt(STOCK_CALC_GET_HISKLINE* pGetHisKLine);

	STOCK_CALC_GET_HISKLINE_RESP* AllocGetHisKLineRespPkt(CStockTraceBase* pTraceBase);
	void PostGetHisKLineRespPkt(STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp);

	STOCK_CALC_GET_CUR_HISKLINE* AllocGetCurHisKLinePkt(CStockTraceBase* pTraceBase);
	void PostGetCurHisKLinePkt(STOCK_CALC_GET_CUR_HISKLINE* pGetCurHisKLine);

	STOCK_CALC_GET_CUR_HISKLINE_RESP* AllocGetCurHisKLineRespPkt(CStockTraceBase* pTraceBase);
	void PostGetCurHisKLineRespPkt(STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp);

	STOCK_CALC_UPDATE_TRACELOG* AllocUpdateTraceLogPkt(CStockTraceBase* pTraceBase);
	void PostUpdateTraceLogPkt(STOCK_CALC_UPDATE_TRACELOG* pUpdateTraceLog);

	STOCK_CALC_UPDATE_TRACELOG_RESP* AllocUpdateTraceLogRespPkt(CStockTraceBase* pTraceBase);
	void PostUpdateTraceLogRespPkt(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp);

	STOCK_CALC_INSERT_TRACERECORD* AllocInsertTraceRecordPkt(CStockTraceBase* pTraceBase);
	void PostInsertTraceRecordPkt(STOCK_CALC_INSERT_TRACERECORD* pInsertTraceRecord);

	STOCK_CALC_INSERT_TRACERECORD_RESP* AllocInsertTraceRecordRespPkt(CStockTraceBase* pTraceBase);
	void PostInsertTraceLRecordRespPkt(STOCK_CALC_INSERT_TRACERECORD_RESP* pInsertTraceRecordResp);

	void AddTraceList(CStockTraceBase* pTraceBase, STOCK_CALC_TRACE_NODE* pTraceNode);

private:
	UINT OnStockAutoManagerInit();
	void OnStockListGetResp(STOCK_CALC_GET_LIST_RESP* pGetListResp);
	BOOL OnStockListGetComplete(int result, void* param, int paramLen);

	UINT OnStockAutoManagerListUpdate();
	void OnStockListUpdateResp(STOCK_CALC_UPDATE_LIST_RESP* pUpdateListResp);
	BOOL OnStockListUpdateComplete(int result, void* param, int paramLen);

	UINT OnStockAutoManagerTraceLogLoad();
	void OnTraceLogLoadResp(STOCK_CALC_LOAD_TRACELOG_RESP* pTraceLogResp);
	BOOL OnTraceLogLoadComplete(int result, void* param, int paramLen);

	UINT OnStockAutoManagerHisKLineUpdate();
	void OnHisKLineUpdateResp(STOCK_CALC_UPDATE_HISKLINE_RESP* pHisKLineResp);
	BOOL OnHisKLineUpdateComplete(int result, void* param, int paramLen);

	void OnHisKLineGetResp(STOCK_CALC_GET_HISKLINE_RESP* pGetHisKLineResp);
	BOOL OnHisKLineGetComplete(int result, void* param, int paramLen);

	void OnUpdateTraceLogResp(STOCK_CALC_UPDATE_TRACELOG_RESP* pUpdateTraceLogResp);
	BOOL OnUpdateTraceLogComplete(int result, void* param, int paramLen);

	void OnCurHisKLineGetResp(STOCK_CALC_GET_CUR_HISKLINE_RESP* pGetCurHisKLineResp);
	BOOL OnCurHisKLineGetComplete(int result, void* param, int paramLen);

	void OnInsertTraceRecordResp(STOCK_CALC_INSERT_TRACERECORD_RESP* pUpdateTraceLogResp);
	BOOL OnInsertTraceRecordComplete(int result, void* param, int paramLen);

	void OnUpdateConfigTrace(STOCK_CALC_UPDATE_CONFIG_TRACE* pUpdateConfigTrace);

	void OnClearTraceHistimeResp(STOCK_CALC_CLEAR_TRACE_HISTIME_RESP* pClearResp);
	BOOL OnClearTraceHistimeComplete(int result, void* param, int paramLen);
	void OnResetTraceCalc(STOCK_CALC_RESET_TRACE_CALC* pResetTrace);

	int OnQueryTraceInfo(QT_STOCK_TRACEINFO_QUERY_PARAM* pQueryTraceInfo);
	UINT OnStockAutoManagerTrace();

	UINT OnStockAutoManagerResetTrace();
private:
	CStockDataTask * m_pDataTask;
	CStockUpdateTask*	m_pUpdateTask;
	CStockTraceBase*	m_pTraceWeek;
	CStockTraceBase*	m_pTraceReal;
	CCriticalSection	m_cs;
	UINT				m_managerStep;
	STOCK_MANAGER_JOB_LIST*		m_pJobList;
	STOCK_MANAGER_JOB_TRACELOG_LOAD*	m_pJobTraceLog;
	STOCK_MANAGER_JOB_LIST_UPDATE	m_jobListUpdate;
	STOCK_MANAGER_JOB_HISKLINE_UPDATE m_jobHisKLineUpdate;
	STOCK_TRACE_JOB_RESET_TRACECLAC m_jobResetTrace;
	DL_LIST							m_listTraceWeek;
	DL_LIST							m_listTraceReal;
	STOCK_CALC_TRACE_NODE*			m_pCalcTraceBuf;
	STOCKAUTO_CONFIG_TRACE			m_traceConfig;
	CStockAutoWindow*				m_pAutoWindow;
	DP_EVENT_ID						m_eventTraceMan;
};

inline CMultiEventsTask* CStockAutoManager::GetTaskData()
{
	return m_pDataTask;
}

inline CMultiEventsTask* CStockAutoManager::GetTaskUpdate()
{
	return m_pUpdateTask;
}


#endif // !__STOCK_AUTO_MANAGER_H__
