/*
*stockAutoManager.h
*/

#ifndef __STOCK_AUTO_MANAGER_H__
#define __STOCK_AUTO_MANAGER_H__

#include <SyncObj.h>
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
	virtual BOOL CheckSelf();
	virtual int OnEventActive(UINT cmd, void* param, int paramLen);
	virtual BOOL OnEventComplete(UINT cmd, int result, void* param, int paramLen);

	enum STOCK_AUTO_MANAGER_STEP
	{
		STOCK_AUTO_MANAGER_STEP_NONE = 0,
		STOCK_AUTO_MANAGER_STEP_LIST_PREPARED,
		STOCK_AUTO_MANAGER_STEP_LIST_UPDATING,
		STOCK_AUTO_MANAGER_STEP_KLINE_PREPARED,
		STOCK_AUTO_MANAGER_STEP_CALC_LOADING,
	};

	

private:
	UINT OnStockAutoManagerInit();
	void OnStockListGetResp(STOCK_CALC_GET_LIST_RESP* pGetListResp);
	BOOL OnStockListGetComplete(int result, void* param, int paramLen);

	UINT OnStockAutoManagerListUpdate();

private:
	CStockDataTask * m_pDataTask;
	CStockUpdateTask*	m_pUpdateTask;
	CCriticalSection	m_cs;
	UINT				m_managerStep;
	STOCK_MANAGER_JOB_LIST*		m_pJobList;
};

#endif // !__STOCK_AUTO_MANAGER_H__
