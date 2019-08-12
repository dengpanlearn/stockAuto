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
		STOCK_AUTO_MANAGER_STEP_LIST_ERROR,
	};

	

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

private:
	CStockDataTask * m_pDataTask;
	CStockUpdateTask*	m_pUpdateTask;
	CCriticalSection	m_cs;
	UINT				m_managerStep;
	STOCK_MANAGER_JOB_LIST*		m_pJobList;
	STOCK_MANAGER_JOB_TRACELOG_LOAD*	m_pJobTraceLog;
	STOCK_MANAGER_JOB_LIST_UPDATE	m_jobListUpdate;
};

#endif // !__STOCK_AUTO_MANAGER_H__
