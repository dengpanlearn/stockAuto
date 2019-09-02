/*
*stockAutoManager.h
*/

#ifndef __STOCK_AUTO_MANAGER_H__
#define __STOCK_AUTO_MANAGER_H__

#include <multiEventsTask.h>
#include "stockCalcDef.h"
#include "stockDataTask.h"
#include "stockUpdateTask.h"

class CStockAutoManager :public CMultiEventsTask
{
public:
	CStockAutoManager();
	virtual ~CStockAutoManager();

	BOOL Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize);
	void Close();

protected:
	virtual void OnExit();
	virtual BOOL CheckSelf();
	virtual int OnEventActive(UINT cmd, void* param, int paramLen);
	virtual BOOL OnEventComplete(UINT cmd, int result, void* param, int paramLen);

	enum STOCK_AUTO_MANAGER_STEP
	{
		STOCK_AUTO_MANAGER_STEP_NONE = 0,
		STOCK_AUTO_MANAGER_STEP_LIST_INIT,
		STOCK_AUTO_MANAGER_STEP_LIST_UPDATING,
		STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING,
		STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING,
		STOCK_AUTO_MANAGER_STEP_STOCK_TRACING,
		STOCK_AUTO_MANAGER_STEP_ERROR,
	};

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
	DL_LIST							m_listTraceWeek;
	DL_LIST							m_listTraceReal;
	STOCK_CALC_TRACE_NODE*			m_pCalcTraceBuf;
};


#endif // !__STOCK_AUTO_MANAGER_H__
