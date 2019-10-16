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

class CMultiEventsTask;

class CQtStockAgent: public CQtTimeAgent
{
	Q_OBJECT
public:
	CQtStockAgent(QObject *parent = NULL);
	virtual ~CQtStockAgent();

	BOOL Create(QThread* pServerObj, CMultiEventsTask* pManager, int timeoutMs);
	void Close();

public:
	void UpdateAutoManagerStep(UINT traceStep, int loadProgress);
	BOOL UpdateStockTrace(char const* pStockName, STOCK_MANAGER_TRACE_LOG* pTraceLog, UINT updateStat);

	void GetAutoManagerLoading(QT_STOCK_LOADING_MANAGER* pLoadingInfo);
	BOOL GetAckStockTrace(QT_STOCK_TRACE_LOG* pTraceLog);
	int	GetStockHisKLine(int count, int offset, STOCK_CALC_TRACE_KLINE* pKLine);

private:
	static BOOL QtTaskEventComplete(UINT cmd, int result, void* param, int paramLen);


	void OnHisKLineQueryResponse(int result, QT_STOCK_HISKLINE_QUERY_PARAM* pHisKLine);

signals:
	void NotifyUiManagerLoadingProgress();
	void NotifyUiStockTrace();
	void NotifyUiHisKLineResponese();

private slots:
	void OnGetQueryHisKLine(QString& code);

protected:
	virtual BOOL OnInit();
	virtual void OnActive();
	virtual void OnTimeout();
	virtual void OnSignalInActive();

	enum
	{
		QT_STOCK_AGENT_UPDATE_MANAGER_STEP = 0x00000001,
		QT_STOCK_AGENT_UPDATE_STOCK_TRACE = 0x00000002,
		QT_STOCK_AGENT_QUERY_HISKLINE_RESPONESE = 0x00000004,
	};

private:
	UINT				m_updateCmd;
	CCriticalSection	m_cs;
	DL_LIST				m_listTraceLog;
	QT_STOCK_HISKLINE_QUERY_JOB*	m_pHisKLineQueryJob;
	QT_STOCK_LOADING_MANAGER		m_loadingManager;
	CMultiEventsTask*		m_pManager;
	CMultiEventsTask*		m_pDataTask;
};

#endif // !__QT_STOCK_AGENT_H__
