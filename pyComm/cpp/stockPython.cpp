/*stockAutoWindow.cpp
*
*/

#include <global.h>
#include "../include/stockPython.h"


CStockPython::CStockPython()
{
	m_pModuleKLine = NULL;
	m_pClassKLine = NULL;
	m_pDictKLine = NULL;
}

CStockPython::~CStockPython()
{
	Release();
}

BOOL CStockPython::Init(char const* pPyDir, char const* pDataDir, int weekCounts)
{
	Py_Initialize();
	if (!Py_IsInitialized())
		return FALSE;

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(pPyDir)");

	do
	{
		m_pModuleKLine = PyImport_ImportModule(PYTHON_MOUDLE_WEEK_KLINE);
		if (m_pModuleKLine == NULL)
			break;

		m_pDictKLine = PyModule_GetDict(m_pModuleKLine);
		if (m_pDictKLine == NULL)
			break;

		m_pClassKLine = PyDict_GetItemString(m_pDictKLine, PYTHON_CLASS_WEEK_KLINE);
		if (m_pClassKLine == NULL)
			break;

		m_pInstanceKLine = PyInstanceMethod_New(m_pClassKLine);
		if (m_pInstanceKLine == NULL)
			break;

		PyObject* pRet = PyObject_CallMethod(m_pClassKLine, PYTHON_CLASS_WEEK_KLINE_METHOD_PREPARE_UPDATE, "s",
			pPyDir, m_pInstanceKLine);

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

	Py_Finalize();
}

BOOL CStockPython::UpdateLatestStockList()
{
	PyObject* pRet = PyObject_CallMethod(m_pClassKLine, "add", "10", "10", m_pInstanceKLine);
}