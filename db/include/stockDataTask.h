/*
*stockDataTask.h
*/

#ifndef  __STOCK_DATA_TASK_H__
#define __STOCK_DATA_TASK_H__

#include <dp.h>
#include <global.h>
#include <stockConfigDef.h>
#include <multiEventsTask.h>
#include "stockDataBase.h"

class CStockDataTask: public CMultiEventsTask
{
public:
	CStockDataTask();
	virtual~CStockDataTask();

	BOOL Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents, int maxEventParamSize);
	void Close();

public:
	BOOL UpdateConfigData();

protected:
	virtual void OnActive();
	virtual int OnEventActive(UINT cmd, void* param, int paramLen);

private:
	void InitConfig(STOCKAUTO_CONFIG_DATA* pConfigData);

private:
	CStockDataBase*		m_pStockData;
	STOCKAUTO_CONFIG_DATA	m_configData;

};
#endif // ! __STOCK_DATA_TASK_H__
