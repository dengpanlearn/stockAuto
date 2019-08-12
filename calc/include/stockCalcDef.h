/*
*stockCalcDef.h
*/

#ifndef __STOCK_CALC_DEF_H__
#define __STOCK_CALC_DEF_H__

#include <taskDef.h>
#define	STOCK_CODE_NAME_MAX				32
#define	STOCK_AUTO_COUNTS_MAX			3800
struct STOCK_CODE_NAME
{
	char	code[STOCK_CODE_NAME_MAX];
	char	name[STOCK_CODE_NAME_MAX];
};

struct STOCK_MANAGER_TRACE_LOG
{
	UINT	traceStep;
	char	code[STOCK_CODE_NAME_MAX];
	long	highTime;
	float	fHighVal;
	long	buyTime;
	float	fBuyVal;
	long	sellTime;
	float	fSellVal;
	int		lastTime;
};

enum STOCK_CALC_EVENT_CMD
{
	STOCK_CALC_EVENT_UPDATE_STOCK_LIST = 0x00000001,
	STOCK_CALC_EVENT_UPDATE_STOCK_LIST_RESP ,
	STOCK_CALC_EVENT_UPDATE_STOCK_KLINE,
	STOCK_CALC_EVENT_GET_STOCK_LIST,
	STOCK_CALC_EVENT_GET_STOCK_LIST_RESP ,
	STOCK_CALC_EVENT_LOAD_TRACE_LOG,
	STOCK_CALC_EVENT_LOAD_TRACE_RESP,
};

typedef TASK_EVENT_PARAM	STOCK_CALC_UPDATE_LIST;

struct STOCK_CALC_UPDATE_LIST_RESP
{
	TASK_EVENT_PARAM	eventParam;
	int					respResult;
};

struct STOCK_CALC_UPDATE_KLINE
{
	TASK_EVENT_PARAM	eventParam;
	char	codeName[STOCK_CODE_NAME_MAX];
	int		updateCycles;
};

struct STOCK_CALC_GET_LIST
{
	TASK_EVENT_PARAM	eventParam;
	UINT				bufCounts;
	STOCK_CODE_NAME*	pListBuf;
};

struct STOCK_CALC_GET_LIST_RESP
{
	TASK_EVENT_PARAM	eventParam;
	int					respResult;
};

struct STOCK_CALC_LOAD_TRACELOG
{
	TASK_EVENT_PARAM	eventParam;
	UINT				bufCounts;
	STOCK_MANAGER_TRACE_LOG*	pLogBuf;
};

struct STOCK_CALC_LOAD_TRACELOG_RESP
{
	TASK_EVENT_PARAM	eventParam;
	int					respResult;
};

struct STOCK_MANAGER_JOB_LIST
{
	UINT    jobStep;
	UINT	stockCounts;
	STOCK_CODE_NAME	codeName[0];
};
struct STOCK_MANAGER_JOB_LIST_UPDATE
{
	UINT    jobStep;
};

struct STOCK_MANAGER_JOB_TRACELOG_LOAD
{
	UINT    jobStep;
	int		logCounts;
	STOCK_MANAGER_TRACE_LOG traceLog[0];
};


#endif // !__STOCK_CALC_DEF_H__
