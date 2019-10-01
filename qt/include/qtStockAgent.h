/*
*qtStockAgent.h
*
*/

#ifndef __QT_STOCK_AGENT_H__
#define __QT_STOCK_AGENT_H__
#include <dp.h>
#include <SyncObj.h>
#include <dllLibEx.h>
#include <stockCalcDef.h>
#include "qtStockTraceDef.h"
#include "qtObjectAgent.h"

class CQtStockAgent: public CQtTimeAgent
{
	Q_OBJECT
public:
	CQtStockAgent(QObject *parent = NULL);
	virtual ~CQtStockAgent();

	BOOL Create(QThread* pServerObj, int timeoutMs);
	void Close();

public:
	void UpdateAutoManagerStep(UINT traceStep);
	BOOL UpdateStockTrace(char const* pStockName, STOCK_MANAGER_TRACE_LOG* pTraceLog, UINT updateStat);

	UINT GetAutoManagerStep();
	BOOL GetAckStockTrace(QT_STOCK_TRACE_LOG* pTraceLog);

signals:
	void NotifyUiManagerStep();
	void NotifyUiStockTrace();

protected:
	virtual BOOL OnInit();
	virtual void OnActive();
	virtual void OnTimeout();

	enum
	{
		QT_STOCK_AGENT_UPDATE_MANAGER_STEP = 0x00000001,
		QT_STOCK_AGENT_UPDATE_STOCK_TRACE = 0x00000002,
	};

private:
	UINT				m_updateCmd;
	CCriticalSection	m_cs;
	UINT				m_autoManagetStep;
	DL_LIST				m_listTraceLog;
};

#endif // !__QT_STOCK_AGENT_H__
