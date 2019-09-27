/*
*qtStockTraceDef.h
*
*/

#ifndef __QT_STOCK_TRACE_DEF_H__
#define __QT_STOCK_TRACE_DEF_H__

#include <stockCalcDef.h>


#define QT_STOCK_TRACE_LOG_STAT_MODIFY		0x0001
#define	QT_STOCK_TRACE_LOG_STAT_ADD			0x0002
#define	QT_STOCK_TRACE_LOG_STAT_DEL			0x0004

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
#endif // !__QT_STOCK_TRACE_DEF_H__
