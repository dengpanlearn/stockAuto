/*
*stockAutoManager.h
*/

#ifndef __STOCK_AUTO_MANAGER_H__
#define __STOCK_AUTO_MANAGER_H__

#include <SyncObj.h>
#include <multiEventsTask.h>
#include "stockCalcDef.h"
#include "stockUpdateTask.h"

class CStockAutoManager :public CMultiEventsTask
{
public:
	CStockAutoManager();
	virtual ~CStockAutoManager();

	BOOL Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents);
	void Close();

protected:
	virtual BOOL CheckSelf();
	virtual int OnEventActive(UINT cmd, void* param);

	enum STOCK_AUTO_MANAGER_STEP
	{
		STOCK_AUTO_MANAGER_STEP_NONE = 0,
		STOCK_AUTO_MANAGER_STEP_LIST_PREPARED,
		STOCK_AUTO_MANAGER_STEP_KLINE_PREPARED,
		STOCK_AUTO_MANAGER_STEP_CALC_LOADING,
	};

private:
	UINT OnStockAutoManagerInit();

private:
	CStockUpdateTask*	m_pUpdateTask;
	CCriticalSection	m_cs;
	UINT				m_managerStep;
	STOCK_MANAGER_JOB_LIST*		m_pJobList;
};

#endif // !__STOCK_AUTO_MANAGER_H__
