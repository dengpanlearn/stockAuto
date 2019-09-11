/*
*stockUpdateBase.cpp
*��Ʊ����ӿ�
*��Ʊ���׽ӿ�
*/

#include "../include/stockAutoManager.h"
#include "../include/stockTraceBase.h"
#include "../include/stockCalcDef.h"
#include "../include/stockUpdateBase.h"

CStockUpdateBase::CStockUpdateBase()
{
	memset(m_szUpdateDir, 0, sizeof(m_szUpdateDir));
}

CStockUpdateBase::~CStockUpdateBase()
{
	Close();
}

BOOL CStockUpdateBase::Create(char const* pUpdateDir)
{
	strncpy(m_szUpdateDir, pUpdateDir, sizeof(m_szUpdateDir) - 1);
	return TRUE;
}

void CStockUpdateBase::Close()
{

}