/*
*stockCalcDef.h
*/

#ifndef __STOCK_CALC_DEF_H__
#define __STOCK_CALC_DEF_H__

#include <dp.h>
#include <dllLibEx.h>
#include <taskDef.h>

#define CALC_IN_DEADZONE(x,zone)			(((x) < (zone)) && ((x) > (-zone)))
#define	STOCK_CODE_NAME_MAX				32
#define	STOCK_AUTO_COUNTS_MAX			3800
#define STOCK_HIS_KLINE_MAX_COUNTS		80
#define STOCK_HIS_KLINE_MAX_COUNTS_FOR_TRACE	75
#define STOCK_HIS_KLINE_MIN_COUNTS_FOR_TRACE	70
#define STOCK_TRACE_DAYS_PER_HIS_UPDATE (7)	

#define STOCK_DAYS_PER_WEEK				7
#define STOCK_SECS_PER_DAY				(24*3600)
#define STOCK_SECS_PER_WEEKS			(STOCK_SECS_PER_DAY*STOCK_DAYS_PER_WEEK)

#define STOCK_CALC_WEEKS_BETWEEN_SECS(start, end) (((end)-(start)+STOCK_SECS_PER_WEEKS/2)/STOCK_SECS_PER_WEEKS)
#define STOCK_CALC_WEEKS_LOST_MAX	10


enum STOCK_AUTO_MANAGER_STEP
{
	STOCK_AUTO_MANAGER_STEP_NONE = 0,
	STOCK_AUTO_MANAGER_STEP_LIST_INIT,
	STOCK_AUTO_MANAGER_STEP_LIST_UPDATING,
	STOCK_AUTO_MANAGER_STEP_TRACELOG_LOADING,
	STOCK_AUTO_MANAGER_STEP_HISKLINE_UPDATING,
	STOCK_AUTO_MANAGER_STEP_STOCK_TRACING,
	STOCK_AUTO_MANAGER_STEP_ERROR,
};

struct STOCK_CODE_NAME
{
	char	code[STOCK_CODE_NAME_MAX];
	char	name[STOCK_CODE_NAME_MAX];
};

struct STOCK_MANAGER_TRACE_LOG
{
	UINT	traceStep;
	char	code[STOCK_CODE_NAME_MAX];
	int		raiseBalanceCheckTimes;
	long	highTime;
	float	fHighVal;
	long	buyTime;
	float	fBuyVal;
	long	sellTime;
	float	fSellVal;
	long	hisTime;
	long	updateTime;
	long	realTime;
};

enum STOCK_CALC_EVENT_CMD
{
	STOCK_CALC_EVENT_UPDATE_STOCK_LIST = 0x00000001,
	STOCK_CALC_EVENT_UPDATE_STOCK_LIST_RESP ,
	STOCK_CALC_EVENT_UPDATE_STOCK_HISKLINE,
	STOCK_CALC_EVENT_UPDATE_STOCK_HISKLINE_RESP,
	STOCK_CALC_EVENT_GET_STOCK_LIST,
	STOCK_CALC_EVENT_GET_STOCK_LIST_RESP ,
	STOCK_CALC_EVENT_LOAD_TRACE_LOG,
	STOCK_CALC_EVENT_LOAD_TRACE_RESP,
	STOCK_CALC_EVENT_UPDATE_TRACE_LOG,
	STOCK_CALC_EVENT_UPDATE_TRACE_LOG_RESP,
	STOCK_CALC_EVENT_GET_STOCK_HISKLINE,
	STOCK_CALC_EVENT_GET_STOCK_HISKLINE_RESP,
	STOCK_CALC_EVENT_GET_CUR_STOCK_HISKLINE,
	STOCK_CALC_EVENT_GET_CUR_STOCK_HISKLINE_RESP,

	STOCK_QT_EVENT_QUERY_STOCK_HISKLINE,
	STOCK_QT_EVENT_QUERY_STOCK_REALKLINE,
};

enum STOCK_CALC_TRACE_STEP
{
	STOCK_CALC_TRACE_STEP_NONE = 0,
	CALC_STOCK_TRADE_STEP_CHECK_BALANCE_RAISE,
	CALC_STOCK_TRADE_STEP_CHECK_HIGH,
	CALC_STOCK_TRADE_STEP_WAIT_BUY,
	CALC_STOCK_TRADE_STEP_WAIT_SELL,
};

struct STOCK_CALC_TRACE_NODE
{
	DL_NODE						node;
	int							stockIdx;
	STOCK_MANAGER_TRACE_LOG*	pTraceLog;
};

struct STOCK_CALC_TRACE_KLINE
{
	long	timeVal;
	float   fOpen;
	float	fClose;
	float	fHigh;
	float	fLow;
	float	fPercent;
	float	fMa10;
	float	fRsi7;
};

enum {
	STOCK_CALC_TRACE_KLINE_INDEX_OPEN = 0, 
	STOCK_CALC_TRACE_KLINE_INDEX_CLOSE,
	STOCK_CALC_TRACE_KLINE_INDEX_HIGH,
	STOCK_CALC_TRACE_KLINE_INDEX_LOW,
	STOCK_CALC_TRACE_KLINE_INDEX_PERCENT,
	STOCK_CALC_TRACE_KLINE_INDEX_MA10,
	STOCK_CALC_TRACE_KLINE_INDEX_TIMEVAL,
	STOCK_CALC_TRACE_KLINE_INDEX_RSI7,
	STOCK_CALC_TRACE_KLINE_INDEX_NUM,
};

typedef TASK_EVENT_PARAM	STOCK_CALC_UPDATE_LIST;

struct STOCK_CALC_UPDATE_LIST_RESP
{
	TASK_EVENT_PARAM	eventParam;
	int					respResult;
};

struct STOCK_CALC_UPDATE_HISKLINE
{
	TASK_EVENT_PARAM	eventParam;
	char	code[STOCK_CODE_NAME_MAX];
	int		updateCycles;
	time_t  endTime;
};

struct STOCK_CALC_UPDATE_HISKLINE_RESP
{
	TASK_EVENT_PARAM	eventParam;
	int		respResult;
};

class CStockTraceBase;
struct STOCK_CALC_GET_CUR_HISKLINE
{
	TASK_EVENT_PARAM	eventParam;
	char	code[STOCK_CODE_NAME_MAX];
	STOCK_CALC_TRACE_KLINE* pCurKLine;
	CStockTraceBase* pTraceBase;
};

struct STOCK_CALC_GET_CUR_HISKLINE_RESP
{
	TASK_EVENT_PARAM	eventParam;
	int		respResult;
	CStockTraceBase* pTraceBase;
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

struct STOCK_CALC_UPDATE_TRACELOG
{
	TASK_EVENT_PARAM	eventParam;
	CStockTraceBase*	pTraceBase;
	STOCK_MANAGER_TRACE_LOG	traceLog;
};


struct STOCK_CALC_UPDATE_TRACELOG_RESP
{
	TASK_EVENT_PARAM	eventParam;
	CStockTraceBase*	pTraceBase;
	int					respResult;
};


class CStockTraceBase;
struct STOCK_CALC_GET_HISKLINE
{
	TASK_EVENT_PARAM	eventParam;
	CStockTraceBase*	pTraceBase;
	STOCK_CALC_TRACE_KLINE*	pKLineBuf;
	char	code[STOCK_CODE_NAME_MAX];
	int		getCnt;
};

struct STOCK_CALC_GET_HISKLINE_RESP
{
	TASK_EVENT_PARAM	eventParam;
	CStockTraceBase*	pTraceBase;
	int respResult;
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

struct STOCK_MANAGER_JOB_HISKLINE_UPDATE
{
	UINT	jobStep;
	int		stockIdx;
	long	lastWeekEndTime;
};

struct STOCK_TRACE_JOB_HISKLINE_GET
{
	UINT	jobStep;
	int		hisEndTime;
	int		hisCounts;
};

struct STOCK_TRACE_JOB_TRACELOG_UPDATE
{
	UINT	jobStep;
	STOCK_MANAGER_TRACE_LOG* pTraceLOG;
};

struct STOCK_TRACE_JOB_CURHISKLINE_GET
{
	UINT	jobStep;
	STOCK_CALC_TRACE_KLINE curKLine;
};

#endif // !__STOCK_CALC_DEF_H__
