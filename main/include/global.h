/*
* global.h
*/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <dp.h>
/*task param*/
#define STOCK_AUTO_MANAGER_TASK_NAME					_T("tAutoMan")
#define STOCK_DATA_TASK_NAME							_T("tData")
#define STOCK_UPDATE_TASK_NAME							_T("tUpdate")

#define	STOCK_AUTO_MANAGER_TASK_PRI						100
#define STOCK_DATA_TASK_PRI								110
#define STOCK_UPDATE_TASK_PRI							110

#define	STOCK_AUTO_MANAGER_TASK_SZ						128*1024
#define STOCK_DATA_TASK_SZ								128*1024
#define STOCK_UPDATE_TASK_SZ							1*1024*1024

#define	STOCK_AUTO_MANAGER_TASK_TIMEOUT					25
#define STOCK_DATA_TASK_TIMEOUT							100
#define STOCK_UPDATE_TASK_TIMEOUT						100

#define	STOCK_AUTO_MANAGER_TASK_EVENTS					512
#define STOCK_DATA_TASK_EVENTS							128
#define STOCK_UPDATE_TASK_EVENTS						128

#define	STOCK_AUTO_MANAGER_TASK_EVENT_PARAM_SIZE		64
#define STOCK_DATA_TASK_EVENT_PARAM_SIZE				64
#define STOCK_UPDATE_TASK_EVENT_PARAM_SIZE				64

/*string name size*/
#define	STOCK_AUTO_SAVE_DIR_MAX_SIZE						128

/*python*/
#define PYTHON_MOUDLE_WEEK_KLINE							"weekLineUpate"
#define	PYTHON_CLASS_WEEK_KLINE								"WeekKLineUpdate"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_PREPARE_UPDATE		"prepareUpdate"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_LIST	"getAndUpdateStockList"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_KLINE	"getAndUpdateWeekKLine"
#endif