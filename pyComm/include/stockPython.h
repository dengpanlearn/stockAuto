/*
* python 脚本调用接口
*/

#ifndef __STOCK_PYTHON_H__
#define __STOCK_PYTHON_H__

#include <dp.h>
#include <Python.h>
#include <stockCalcDef.h>
class CStockPython
{
public:
	CStockPython();
	~CStockPython();

	BOOL Init(char const* pPyDir, char const* pModule, char const* pDataDir, char const* pStockListDb, char const* pStockKLineDb);
	void Release();

public:
	BOOL UpdateLatestStockList();

	BOOL UpdateLatestKLine(char const* pStockCode, int counts);
	BOOL UpdateLatestKLineByTime(char const* pStockCode, time_t endTime, int counts);

	BOOL GetLatestKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pCurKLine);

private:
	PyObject*	m_pModuleKLine;
	PyObject*	m_pDictKLine;
	PyObject*	m_pClassKLine;
	PyObject*	m_pInstanceKLine;
};

#endif // !__STOCK_PYTHON_H__



