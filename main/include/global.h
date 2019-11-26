/*
* global.h
*/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <dp.h>

#define	STOCK_AUTO_MANAGER_EXIT_CHECK_TIMEOUT			(100)

/*task param*/
#define STOCK_AUTO_MANAGER_TASK_NAME					_T("tAutoMan")
#define STOCK_DATA_TASK_NAME							_T("tData")
#define STOCK_UPDATE_TASK_NAME							_T("tUpdate")
#define STOCK_CONFIG_TASK_NAME							_T("tConfig")

#define	STOCK_AUTO_MANAGER_TASK_PRI						100
#define STOCK_DATA_TASK_PRI								110
#define STOCK_UPDATE_TASK_PRI							110
#define STOCK_CONFIG_TASK_PRI							110

#define	STOCK_AUTO_MANAGER_TASK_SZ						128*1024
#define STOCK_DATA_TASK_SZ								128*1024
#define STOCK_UPDATE_TASK_SZ							1*1024*1024
#define STOCK_CONFIG_TASK_SZ							128*1024

#define	STOCK_AUTO_MANAGER_TASK_TIMEOUT					100
#define	STOCK_AUTO_MANAGER_TASK_TIMEOUT_TRACE			30
#define STOCK_DATA_TASK_TIMEOUT							100
#define STOCK_UPDATE_TASK_TIMEOUT						100
#define STOCK_CONFIG_TASK_TIMEOUT						1000
#define QT_STOCK_AGENT_UPDATE_TIMEOUT					500

#define	STOCK_AUTO_MANAGER_TASK_EVENTS					512
#define STOCK_DATA_TASK_EVENTS							128
#define STOCK_UPDATE_TASK_EVENTS						128

#define	STOCK_AUTO_MANAGER_TASK_EVENT_PARAM_SIZE		128
#define STOCK_DATA_TASK_EVENT_PARAM_SIZE				128
#define STOCK_UPDATE_TASK_EVENT_PARAM_SIZE				128
#define STOCK_CONFIG_FILE_NAME							"./config.ini"
/*string name size*/
#define	STOCK_AUTO_SAVE_DIR_MAX_SIZE						128

/*python*/
#define PYTHON_MOUDLE_WEEK_KLINE							"weekLineUpate"
#define	PYTHON_CLASS_WEEK_KLINE								"WeekKLineUpdate"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_PREPARE_UPDATE		"prepareUpdate"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_LIST	"getAndUpdateStockList"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_UPDATE_STOCK_KLINE	"getAndUpdateWeekKLine"
#define	PYTHON_CLASS_WEEK_KLINE_METHOD_LATEST_STOCK_KLINE	"getCurWeekKLine"


#ifdef __cplusplus
extern "C"{
#endif

class CStockAutoWindow;
BOOL GlobalInit(CStockAutoWindow* pAutoWindow);
void GlobalExit();
#ifdef __cplusplus
}
#endif
#endif