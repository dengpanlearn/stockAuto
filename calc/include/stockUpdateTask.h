/*
*stockUpdateTask.h
*/

#ifndef __STOCK_UPDATE_TASK_H__
#define __STOCK_UPDATE_TASK_H__

#include <multiEventsTask.h>
#include "stockConfigDef.h"
#include "stockUpdateBase.h"

class CStockUpdateTask : public CMultiEventsTask
{
public: 
	CStockUpdateTask();
	virtual~CStockUpdateTask();

	BOOL Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, int maxEvents);
	void Close();

protected:
	virtual void OnActive();
	virtual int OnEventActive(UINT cmd, void* param);

private:
	void InitConfig(STOCKAUTO_CONFIG_PYTHON* pConfigPython);

protected:
	CStockUpdateBase *			m_pStockUpdate;
	STOCKAUTO_CONFIG_PYTHON		m_configPython;
};
#endif // !__STOCK_UPDATE_TASK_H__
