/*
*qtStockTraceDef.h
*
*/

#ifndef __QT_STOCK_TRACE_DEF_H__
#define __QT_STOCK_TRACE_DEF_H__

#include <stockCalcDef.h>

#define QT_STOCK_SERVER_EXIT_WAIT_TIMEOUT	1000
#define QT_STOCK_TRACE_LOG_STAT_MODIFY		0x0100
#define	QT_STOCK_TRACE_LOG_STAT_ADD			0x0200
#define	QT_STOCK_TRACE_LOG_STAT_DEL			0x0400
#define QT_STOCK_QUERY_HISKLINE_COUNTS		STOCK_HIS_KLINE_MIN_COUNTS_FOR_TRACE

enum
{
	QT_STOCK_TRACE_LOG_STAT_RAISE_BALANCED = 0x01,
	QT_STOCK_TRACE_LOG_STAT_HIGH_REACHED,
	QT_STOCK_TRACE_LOG_STAT_RSI_REACHED,
	QT_STOCK_TRACE_LOG_STAT_BUYING,
	QT_STOCK_TRACE_LOG_STAT_BUYED,
	QT_STOCK_TRACE_LOG_STAT_SELLING,
	QT_STOCK_TRACE_LOG_STAT_SELLED,
};

struct QT_STOCK_TRACE_LOG
{
	UINT		stat;
	char	stockName[STOCK_CODE_NAME_MAX];
	char	code[STOCK_CODE_NAME_MAX];
};


struct QT_STOCK_TRACE_LOG_NODE
{
	DL_NODE		node;
	QT_STOCK_TRACE_LOG	traceLogVal;
};

struct QT_STOCK_HISKLINE_QUERY_JOB
{
	int		jobRsult;
	BOOL	bInWorking;
	int		hisCounts;
	char	code[STOCK_CODE_NAME_MAX];
	STOCK_CALC_TRACE_KLINE hisKLine[STOCK_HIS_KLINE_MAX_COUNTS];
};

struct QT_STOCK_REALKLINE_INFO
{
	char	name[STOCK_CODE_NAME_MAX];
	char	code[STOCK_CODE_NAME_MAX];
	STOCK_CALC_TRACE_KLINE realKLine;
};

struct QT_STOCK_REALKLINE_QUERY_JOB
{
	int		jobResult;
	BOOL	bInWorking;
	QT_STOCK_REALKLINE_INFO realKLine;
};

struct QT_STOCK_TRACE_INFO
{
	char	name[STOCK_CODE_NAME_MAX];
	char	code[STOCK_CODE_NAME_MAX];
	UINT					traceStep;
	long					highTime;
	float					fHighVal;
	long					buyTime;
	float					fBuyVal;
	long					topTime;
	float					fTopVal;
	long					sellTime;
	float					fSellVal;
};

struct QT_STOCK_TRACEINFO_QUERY_JOB
{
	int					jobResult;
	BOOL				bInWorking;
	QT_STOCK_TRACE_INFO traceInfo;
};

struct QT_STOCK_SELLSTAT_INFO
{
	QT_STOCK_TRACE_INFO     traceInfo;
	STOCK_CALC_TRACE_KLINE	realKline;
};

struct QT_STOCK_SELLSTAT_INFO_EXE
{
	char	name[STOCK_CODE_NAME_MAX];
	char	code[STOCK_CODE_NAME_MAX];
	UINT					traceStep;
	float					fBuyVal;
	float					fTopVal;
	float					fCurVal;
	float					fRsi;
	float					fMa;
};

struct QT_STOCK_SELLSTAT_QUERY_JOB
{
	int					jobResult;
	BOOL				bInWorking;
	QT_STOCK_SELLSTAT_INFO sellStatInfo;
};

struct QT_STOCK_LOADING_MANAGER
{
	UINT	step;
	int		progress;
};

struct QT_STOCK_RESET_TRACE_JOB
{
	int		jobResult;
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


struct QT_STOCK_REALKLINE_QUERY_PARAM
{
	TASK_EVENT_PARAM		eventParam;
	char					code[STOCK_CODE_NAME_MAX];
	STOCK_CALC_TRACE_KLINE* pRealKLineBuf;
	CQtStockAgent*		pStockAgent;
};

struct QT_STOCK_TRACEINFO_QUERY_PARAM
{
	TASK_EVENT_PARAM		eventParam;
	char					code[STOCK_CODE_NAME_MAX];
	CQtStockAgent*			pStockAgent;
	UINT					traceStep;
	long					highTime;
	float					fHighVal;
	long					buyTime;
	float					fBuyVal;
	long				    topTime;
	float					fTopVal;
	long					sellTime;
	float					fSellVal;
};


#endif // !__QT_STOCK_TRACE_DEF_H__
