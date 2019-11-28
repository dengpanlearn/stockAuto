/*
*stockConfigTask.cpp
*/

#include "../include/stockConfigTask.h"

CStockConfigTask::CStockConfigTask()
{
	memset(&m_updateMask, 0, sizeof(m_updateMask));
	memset(&m_szFileName, 0, sizeof(m_szFileName));
}

CStockConfigTask::~CStockConfigTask()
{
	Close();
}

BOOL CStockConfigTask::Create(LPCTSTR pNameTask, int stackSize, int priTask, int optTask, int timeoutMs, char const* pFileName)
{
	if (!m_config.LoadConfig(pFileName))
		return FALSE;

	strncpy(m_szFileName, pFileName, sizeof(m_szFileName));

	return CTimeOutTask::Create(pNameTask, stackSize, priTask, optTask, timeoutMs);
}

void CStockConfigTask::Close()
{
	return CTimeOutTask::Close();
}

void CStockConfigTask::GetConfigPython(STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	CSingleLock(&m_cs, TRUE);
	STOCKAUTO_CONFIG_PYTHON const* pConfigPythonPtr = m_config.GetConfigPythonPtr();
	memcpy(pConfigPython, pConfigPythonPtr, sizeof(STOCKAUTO_CONFIG_PYTHON));
}

void CStockConfigTask::GetConfigData(STOCKAUTO_CONFIG_DATA* pConfigData)
{
	CSingleLock(&m_cs, TRUE);
	STOCKAUTO_CONFIG_DATA const* pConfigDataPtr = m_config.GetConfigDataPtr();
	memcpy(pConfigData, pConfigDataPtr, sizeof(STOCKAUTO_CONFIG_DATA));
}

void CStockConfigTask::GetConfigTrace(STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	CSingleLock(&m_cs, TRUE);
	STOCKAUTO_CONFIG_TRACE const* pConfigTracePtr = m_config.GetConfigTracePtr();
	memcpy(pConfigTrace, pConfigTracePtr, sizeof(STOCKAUTO_CONFIG_TRACE));
}

void CStockConfigTask::UpdateConfigPython(STOCKAUTO_CONFIG_PYTHON const* pConfigPython)
{
	CSingleLock(&m_cs, TRUE);
	m_config.UpdateConfigPython(pConfigPython);

	m_updateMask |= STOCK_CONFIG_UPDATE_PYTHON;
}

void CStockConfigTask::UpdateConfigData(STOCKAUTO_CONFIG_DATA const* pConfigData)
{
	CSingleLock(&m_cs, TRUE);
	m_config.UpdateConfigData(pConfigData);

	m_updateMask |= STOCK_CONFIG_UPDATE_DATA;
}

void CStockConfigTask::UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	CSingleLock(&m_cs, TRUE);
	m_config.UpdateConfigTrace(pConfigTrace);

	m_updateMask |= STOCK_CONFIG_UPDATE_TRACE;
}

void CStockConfigTask::SyncConfig()
{
	CSingleLock(&m_cs, TRUE);
	m_updateMask |= STOCK_CONFIG_UPDATE_SYNC;
}

void CStockConfigTask::OnTimeout()
{
	CSingleLock(&m_cs, TRUE);
	if (m_updateMask & STOCK_CONFIG_UPDATE_SYNC)
	{
		m_config.SaveConfig(m_szFileName);
		m_updateMask = 0;
	}
}