/*
*qtStockAgent.cpp
*
*/

#include <stockCalcDef.h>
#include "../include/qtStockTraceDef.h"
#include "../include/qtStockAgent.h"

CQtStockAgent::CQtStockAgent(QObject* parent): CQtTimeAgent(parent)
{

}

CQtStockAgent::~CQtStockAgent()
{
	Close();
}

BOOL CQtStockAgent::Create(QThread* pServerObj, int timeoutMs)
{
	return CQtTimeAgent::Create(pServerObj, timeoutMs);
}

void CQtStockAgent::Close()
{

}

void CQtStockAgent::UpdateAutoManagerStep(UINT traceStep)
{
	CSingleLock lock(&m_cs, TRUE);
	m_updateCmd |= QT_STOCK_AGENT_UPDATE_MANAGER_STEP;
	m_autoManagetStep = traceStep;
}

BOOL CQtStockAgent::UpdateStockTrace(char const* pStockName, STOCK_MANAGER_TRACE_LOG* pTraceLog, UINT updateStat)
{
	QT_STOCK_TRACE_LOG_NODE* pTraceLogNode = (QT_STOCK_TRACE_LOG_NODE*)calloc(sizeof(QT_STOCK_TRACE_LOG_NODE), 1);
	if (pTraceLogNode == NULL)
		return FALSE;

	pTraceLogNode->traceLogVal.stat = updateStat;
	strncpy(pTraceLogNode->traceLogVal.stockName, pStockName, STOCK_CODE_NAME_MAX);
	memcpy(&pTraceLogNode->traceLogVal.traceLog, pTraceLog, sizeof(pTraceLog));

	CSingleLock lock(&m_cs, TRUE);
	m_updateCmd |= QT_STOCK_AGENT_UPDATE_STOCK_TRACE;
	dllInsert(&m_listTraceLog, NULL, &pTraceLogNode->node);

	return TRUE;
}


UINT CQtStockAgent::GetAutoManagerStep()
{
	CSingleLock lock(&m_cs);
	return m_autoManagetStep;
}

BOOL CQtStockAgent::GetAckStockTrace(QT_STOCK_TRACE_LOG* pTraceLog)
{
	union
	{
		QT_STOCK_TRACE_LOG_NODE* pTraceLogNode;
		DL_NODE*		pNode;
	};
	
	CSingleLock lock(&m_cs, TRUE);
	pNode = dllGet(&m_listTraceLog);
	if (pNode == NULL)
		return FALSE;

	memcpy(pTraceLog, &pTraceLogNode->traceLogVal, sizeof(QT_STOCK_TRACE_LOG));
	free(pTraceLogNode);
	return TRUE;
}

BOOL CQtStockAgent::OnInit()
{
	dllInit(&m_listTraceLog);
	m_autoManagetStep = 0;
	return CQtTimeAgent::OnInit();
}

void CQtStockAgent::OnActive()
{

}

void CQtStockAgent::OnTimeout()
{
	CSingleLock lock(&m_cs, TRUE);
	if (m_updateCmd & QT_STOCK_AGENT_UPDATE_MANAGER_STEP)
	{
		m_updateCmd = 0;
		emit NotifyUiManagerStep();
	}

	if (!DLL_IS_EMPTY(&m_listTraceLog))
		emit NotifyUiStockTrace();
}