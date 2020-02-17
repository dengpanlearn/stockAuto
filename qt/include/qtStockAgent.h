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
#include <stockConfigTask.h>
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
	BOOL UpdateStockTrace(char const* pStockName, char const* pStockCode, UINT updateStat);

	void GetAutoManagerLoading(QT_STOCK_LOADING_MANAGER* pLoadingInfo);
	int GetAckStockTrace(QT_STOCK_TRACE_LOG* pTraceLog, int counts);
	int	GetStockHisKLine(int count, int offset, STOCK_CALC_TRACE_KLINE* pKLine);
	BOOL GetStockRealKLine(QT_STOCK_REALKLINE_INFO* pKLineInfo, long long* pPrevVolBuf, int& volCounts);
	BOOL GetStockTraceInfo(QT_STOCK_TRACE_INFO* pTraceInfo);
	BOOL GetStockSellStat(QT_STOCK_SELLSTAT_INFO_EXE* pSellStat);
	int	 GetStockTraceRecord(QT_STOCK_TRACERECORD_INFO* pBuf, int offset, int counts);
	void GetConfigPython(STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void GetConfigData(STOCKAUTO_CONFIG_DATA* pConfigData);
	void GetConfigTrace(STOCKAUTO_CONFIG_TRACE* pConfigTrace);
	
	void UpdateConfigPython(STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void UpdateConfigData(STOCKAUTO_CONFIG_DATA* pConfigData);
	void UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE* pConfigTrace);
	void SyncConfig();

	void UpdateResetTraceResult(int result);

private:
	static BOOL QtTaskEventComplete(UINT cmd, int result, void* param, int paramLen);


	void OnHisKLineQueryResponse(int result, QT_STOCK_HISKLINE_QUERY_PARAM* pHisKLine);
	void OnRealKLineQueryResponse(int result, QT_STOCK_REALKLINE_QUERY_PARAM* pRealKLine);
	void OnRealKLinePrevKLineQueryResponse(int result, QT_STOCK_HISKLINE_QUERY_PARAM* pRealKLine);
	void OnTraceInfoQueryResponse(int result, QT_STOCK_TRACEINFO_QUERY_PARAM* pTraceInfo);
	void OnSellStatQueryRealKLineResponse(int result, QT_STOCK_REALKLINE_QUERY_PARAM* pRealKLine);
	void OnSellStatQueryTraceInfoResponse(int result, QT_STOCK_TRACEINFO_QUERY_PARAM* pTraceInfo);
	void OnTraceRecordQueryResponse(int result, QT_STOCK_TRACERECORD_QUERY_PARAM* pTraceRecord);

signals:
	void NotifyUiManagerLoadingProgress();
	void NotifyUiStockTrace();
	void NotifyUiHisKLineResponese();
	void NotifyUiRealKLineResponese();
	void NotifyResetTraceResponese();
	void NotifyUiTraceInfoResponse();
	void NotifyUiSellStatResponse();
	void NotifyUiTraceRecordResponse();
	
private slots:
	void OnGetQueryHisKLine(QString& code);
	void OnGetQueryRealKLine(QString& code, QString& name);
	void OnGetQueryTraceInfo(QString& code, QString& name);
	void OnGetQuerySellStat(QString& code, QString& name);
	void OnRequestResetTrace();
	void OnGetQueryTraceRecord(int start, int end, int offset);

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
		QT_STOCK_AGENT_QUERY_REALKLINE_RESPONESE = 0x00000008,
		QT_STOCK_AGENT_REQUEST_RESET_TRACE_RESPONESE = 0x00000010,
		QT_STOCK_AGENT_QUERY_TRACEINFO_RESPONESE = 0x00000020,
		QT_STOCK_AGENT_QUERY_SELLSTAT_RESPONESE = 0x00000040,
		QT_STOCK_AGENT_QUERY_TRACERECORD_RESPONESE = 0x00000080,
	};

private:
	UINT				m_updateCmd;
	CCriticalSection	m_cs;
	DL_LIST				m_listTraceLog;
	QT_STOCK_HISKLINE_QUERY_JOB*	m_pHisKLineQueryJob;
	QT_STOCK_REALKLINE_QUERY_JOB	m_realKLineQueryJob;
	QT_STOCK_TRACEINFO_QUERY_JOB	m_traceInfoQueryJob;
	QT_STOCK_TRACERECORD_QUERY_JOB*	m_pTraceRecordQueryJob;
	QT_STOCK_RESET_TRACE_JOB		m_resetTraceJob;
	QT_STOCK_LOADING_MANAGER		m_loadingManager;
	QT_STOCK_SELLSTAT_QUERY_JOB		m_sellStatQueryJob;
	CMultiEventsTask*		m_pManager;
	CMultiEventsTask*		m_pDataTask;
	CMultiEventsTask*		m_pUpdateTask;
};

#endif // !__QT_STOCK_AGENT_H__
