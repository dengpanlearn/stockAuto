/*
*qtStockTraceDef.h
*
*/

#ifndef __QT_STOCK_TRACE_DEF_H__
#define __QT_STOCK_TRACE_DEF_H__

#include <stockCalcDef.h>

#define QT_STOCK_SERVER_EXIT_WAIT_TIMEOUT	1000
#define QT_STOCK_TRACE_LOG_STAT_MODIFY		0x0001
#define	QT_STOCK_TRACE_LOG_STAT_ADD			0x0002
#define	QT_STOCK_TRACE_LOG_STAT_DEL			0x0004
#define QT_STOCK_QUERY_HISKLINE_COUNTS		STOCK_HIS_KLINE_MIN_COUNTS_FOR_TRACE
		

struct QT_STOCK_TRACE_LOG
{
	UINT		stat;
	char stockName[STOCK_CODE_NAME_MAX];
	STOCK_MANAGER_TRACE_LOG	traceLog;
};


struct QT_STOCK_TRACE_LOG_NODE
{
	DL_NODE		node;
	QT_STOCK_TRACE_LOG	traceLogVal;
};

struct QT_STOCK_HISKLINE_QUERY_JOB
{
	int		jobRsult;
	int		hisCounts;
	char	code[STOCK_CODE_NAME_MAX];
	STOCK_CALC_TRACE_KLINE hisKLine[STOCK_HIS_KLINE_MAX_COUNTS];
};


struct QT_STOCK_LOADING_MANAGER
{
	UINT	step;
	int		progress;
};

class CQtStockAgent;
struct QT_STOCK_HISKLINE_QUERY_PARAM
{
	TASK_EVENT_PARAM		eventParam;
	int						hisCounts;
	char					code[STOCK_CODE_NAME_MAX];
	STOCK_CALC_TRACE_KLINE* pHisKLineBuf;
	CQtStockAgent*		pStockAgent;
};


#endif // !__QT_STOCK_TRACE_DEF_H__
