/*stockAutoWindow.cpp
*
*/

#include <global.h>
#include "../include/stockConfigDef.h"
#include "../include/stockPython.h"


CStockPython::CStockPython()
{
	m_pModuleKLine = NULL;
	m_pClassKLine = NULL;
	m_pDictKLine = NULL;
	m_pInstanceKLine = NULL;
}

CStockPython::~CStockPython()
{
	Release();
}

BOOL CStockPython::Init(char const* pPyDir, char const* pModule, char const* pDataDir, char const* pStockListDb, char const* pStockKLineDb)
{
	Py_Initialize();
	if (!Py_IsInitialized())
		return FALSE;

	PyRun_SimpleString("import sys");
	char szTmp[64] = { 0 };
	sprintf_s(szTmp, sizeof(szTmp)-1, "sys.path.append('%s')", pPyDir);
	PyRun_SimpleString(szTmp);

	do
	{
		m_pModuleKLine = PyImport_ImportModule(pModule);
		if (m_pModuleKLine == NULL)
			break;

		m_pDictKLine = PyModule_GetDict(m_pModuleKLine);
		if (m_pDictKLine == NULL)
			break;

		m_pClassKLine = PyDict_GetItemString(m_pDictKLine, PYTHON_CLASS_WEEK_KLINE);
		if (m_pClassKLine == NULL)
			break;

		m_pInstanceKLine = PyObject_CallObject(m_pClassKLine, NULL);
		if (m_pInstanceKLine == NULL)
			break;

		PyObject* pRet = PyObject_CallMethod(m_pInstanceKLine, PYTHON_CLASS_WEEK_KLINE_METHOD_PREPARE_UPDATE, "sss",
			pDataDir, pStockListDb, pStockKLineDb);
		if (pRet == NULL)
			break;

		int result = -1;
		PyArg_Parse(pRet, "i", &result);
		Py_DECREF(pRet);

		if (result < 0)
			break;

		return TRUE;
	} while (FALSE);

	Release();
	return FALSE;
}

void CStockPython::Release()
{
#if 1
	if (m_pInstanceKLine != NULL)
	{
		Py_DECREF(m_pInstanceKLine);
		m_pInstanceKLine = NULL;
	}

	if (m_pClassKLine != NULL)
	{
		Py_DECREF(m_pClassKLine);
		m_pClassKLine = NULL;
	}

	if (m_pDictKLine != NULL)
	{
		Py_DECREF(m_pDictKLine);
		m_pDictKLine = NULL;
	}

	if (m_pModuleKLine != NULL)
	{
		Py_DECREF(m_pModuleKLine);
		m_pModuleKLine = NULL;
	}
#endif
	Py_Finalize();
}

BOOL CStockPython::UpdateLatestStockList()
{
	time_t curTime = time(NULL);
	PyObject* pRet = PyObject_CallMethod(m_pInstanceKLine, PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_LIST, "L", curTime);
	if (pRet == NULL)
		return FALSE;

	int result = -1;
	PyArg_Parse(pRet, "i", &result);
	Py_DECREF(pRet);
	return result == 0;
}

BOOL CStockPython::UpdateLatestKLine(char const* pStockCode, int counts)
{
	time_t curTime = time(NULL);

	PyObject* pRet = PyObject_CallMethod(m_pInstanceKLine, PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_KLINE, "sLi", pStockCode, curTime, counts);
	if (pRet == NULL)
		return FALSE;

	int result = -1;
	PyArg_Parse(pRet, "i", &result);
	Py_DECREF(pRet);
	return result == 0;
}

BOOL CStockPython::UpdateLatestKLineByTime(char const* pStockCode, time_t endTime, int counts)
{
	PyObject* pRet = PyObject_CallMethod(m_pInstanceKLine, PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_KLINE, "sLi", pStockCode, endTime, counts);
	if (pRet == NULL)
		return FALSE;

	int result = -1;
	PyArg_Parse(pRet, "i", &result);
	Py_DECREF(pRet);
	return result == 0;
}

BOOL CStockPython::GetLatestKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pCurKLine)
{
	PyObject* pItemVal;
	int itemSize = 0;
	
	PyObject* pCode = NULL;
	PyObject* pItem = NULL;
	
	PyObject* pRet = PyObject_CallMethod(m_pInstanceKLine, PYTHON_CLASS_WEEK_KLINE_METHOD_LATEST_STOCK_KLINE, "s", pStockCode);
	if (pRet == NULL)
		return FALSE;

	int parseResult  = -1;
	BOOL bResult = FALSE;
	pCode = PyDict_GetItemString(pRet, "code");
	if (pCode == NULL)
		goto _CODE_NONE;

	PyArg_Parse(pCode, "i", &parseResult);
	if (parseResult == -1)
		goto _CODE_ERROR;

	pItem = PyDict_GetItemString(pRet, "item");
	if (pItem == NULL)
		goto _CODE_NONE;

	if (!PyList_Check(pItem))
		goto _ITEM_ERROR;

	itemSize = PyList_Size(pItem);
	if (itemSize != STOCK_CALC_TRACE_KLINE_INDEX_NUM)
		goto _ITEM_ERROR;

	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_OPEN);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fOpen);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_CLOSE);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fClose);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_HIGH);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fHigh);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_LOW);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fLow);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_PERCENT);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fPercent);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_MA10);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fMa10);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_TIMEVAL);
	PyArg_Parse(pItemVal, "i", &pCurKLine->timeVal);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_VOLUME);
	PyArg_Parse(pItemVal, "i", &pCurKLine->volume);
	
	pItemVal = PyList_GetItem(pItem, STOCK_CALC_TRACE_KLINE_INDEX_RSI7);
	PyArg_Parse(pItemVal, "f", &pCurKLine->fRsi7);
	
	bResult = TRUE;
_ITEM_ERROR:
	Py_DECREF(pItem);
_ITEM_NONE:
_CODE_ERROR:
	Py_DECREF(pCode);
_CODE_NONE:
	Py_DECREF(pRet);
	return bResult;
}