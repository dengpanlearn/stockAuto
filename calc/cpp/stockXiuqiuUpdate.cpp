/*
*stockXiuqiuUpdate.cpp
*xiuqiu网提供的https股票行情接口
*股票交易接口
*/

#include "../include/stockXueqiuUpdate.h"

CStockXiuQiuUpdate::CStockXiuQiuUpdate()
{
	pStockPython = NULL;
	memset(m_szModuleDir, 0, sizeof(m_szModuleDir));
}

CStockXiuQiuUpdate::~CStockXiuQiuUpdate()
{
	Close();
}

BOOL CStockXiuQiuUpdate::Create(char const* pUpdateDir, char const* pStockListDb, char const* pStockKLineDb, char const* pModuleDir, char const* pModule)
{
	if (!CStockUpdateBase::Create(pUpdateDir))
		return FALSE;

	pStockPython = new CStockPython();
	if (pStockPython == NULL)
		return FALSE;

	if (!pStockPython->Init(pModuleDir, pModule, pUpdateDir, pStockListDb, pStockKLineDb))
	{
		delete pStockPython;
		pStockPython = NULL;
		return FALSE;
	}

	strncpy(m_szModuleDir, pModuleDir, sizeof(pModuleDir) - 1);
	return TRUE;
}

void CStockXiuQiuUpdate::Close()
{
	if (!pStockPython)
	{
		pStockPython->Release();
		delete pStockPython;
		pStockPython = NULL;
	}
}

BOOL CStockXiuQiuUpdate::UpdateLatestStockList()
{
	return pStockPython->UpdateLatestStockList();
}

BOOL CStockXiuQiuUpdate::UpdateLatestKLine(char const* pStockCode, int counts)
{
	return pStockPython->UpdateLatestKLine(pStockCode, counts);
}