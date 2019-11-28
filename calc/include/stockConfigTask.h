/*
*stockConfigTask
*/

#ifndef __STOCK_TASK_CONFIG_TASK_H
#define __STOCK_TASK_CONFIG_TASK_H

#include <dp.h>
#include <timeOutTask.h>
#include <SyncObj.h>
#include "stockConfig.h"

class CStockConfigTask : public CTimeOutTask
{
public:
	CStockConfigTask();
	~CStockConfigTask();

	BOOL Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, char const* pFileName);
	void Close();

public:
	void GetConfigPython(STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void GetConfigData(STOCKAUTO_CONFIG_DATA* pConfigData);
	void GetConfigTrace(STOCKAUTO_CONFIG_TRACE* pConfigTrace);

	void UpdateConfigPython(STOCKAUTO_CONFIG_PYTHON const* pConfigPython);
	void UpdateConfigData(STOCKAUTO_CONFIG_DATA const* pConfigData);
	void UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace);

	void SyncConfig();
protected:
	virtual void OnTimeout();

private:
	enum
	{
		STOCK_CONFIG_UPDATE_PYTHON = 0x01,
		STOCK_CONFIG_UPDATE_DATA = 0x02,
		STOCK_CONFIG_UPDATE_TRACE = 0x04,
		STOCK_CONFIG_UPDATE_SYNC = 0x08
	};

	CStockConfig	m_config;
	CCriticalSection	m_cs;
	UINT		m_updateMask;
	char		m_szFileName[STOCK_CONFIG_NAME_MAX];
};

#endif // !__STOCK_TASK_CONFIG_TASK_H
