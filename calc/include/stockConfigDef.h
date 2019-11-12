/*stockConfigDef.h
*
*/

#ifndef __STOCK_CONFIG_DEF_H__
#define __STOCK_CONFIG_DEF_H__

#include <global.h>
#define STOCK_CONFIG_NAME_MAX		STOCK_AUTO_SAVE_DIR_MAX_SIZE

#define XXX_WORK

/*默认配置*/
#ifdef XXX_WORK
#define STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT		"E:\\self\\stock\\src\\py"
#else
#define STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT		"E:\\work\\stock\\StockAuto\\src\\py"
#endif 
#define	STOCKAUTO_CONFIG_PYTHON_KLINE_MODULE_DFT	PYTHON_MOUDLE_WEEK_KLINE
#ifdef XXX_WORK
#define	STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR_DFT			"E:\\self\\stock\\data"
#else
#define STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR_DFT		"E:\\work\\stock\\StockAuto\\data"
#endif
#define	STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST_DFT		"stockList.db"
#define	STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE_DFT		"kline.db"

struct STOCKAUTO_CONFIG_PYTHON
{
	char moduleDir[STOCK_CONFIG_NAME_MAX];			/*python 模块路径*/
	char klineModule[STOCK_CONFIG_NAME_MAX];		/*python K线更新模块名称*/
	char updateDir[STOCK_CONFIG_NAME_MAX];			/*python 获取数据更新路径*/
	char dbStockList[STOCK_CONFIG_NAME_MAX];		/*python 获取股票列表数据库名称*/
	char dbStockKLine[STOCK_CONFIG_NAME_MAX];		/*python 获取股票KLine数据库名称*/
};

#define STOCKAUTO_CONFIG_DATA_DATADIR_DFT			STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR_DFT
#define	STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT		STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST_DFT
#define	STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT	STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE_DFT
#define	STOCKAUTO_CONFIG_DATA_TRACELOG_FILENAME_DFT	"tracelog.db"

struct STOCKAUTO_CONFIG_DATA
{
	char dataDir[STOCK_CONFIG_NAME_MAX];
	char listFileName[STOCK_CONFIG_NAME_MAX];
	char klineFileName[STOCK_CONFIG_NAME_MAX];
	char tracelogFileName[STOCK_CONFIG_NAME_MAX];
};

struct STOCKAUTO_CONFIG_TRACE
{
	float		fRaisePercent;
	int			raiseBalances;
	int			reachHighRanges;
	int			rsiBuyWaits;
	float		fRsiBuy;
	float		fRsiSell;
	float		rsiSellWaits; 
	float		fCutLossPercent;
};

#define STOCKAUTO_CONFIG_TRACE_RAISE_PERCENT_DFT		0.9
#define STOCKAUTO_CONFIG_TRACE_RAISE_BALANCES_DFT		3
#define STOCKAUTO_CONFIG_TRACE_REAHHIGH_RANGES_DFT		60
#define STOCKAUTO_CONFIG_TRACE_RSI_BUY_WAIT_DFT			5
#define STOCKAUTO_CONFIG_TRACE_RSI_BUY_DFT				75
#define STOCKAUTO_CONFIG_TRACE_RSI_SELL_DFT				70
#define	STOCKAUTO_CONFIG_TRACE_RSI_SELL_WAITS_DFT		3
#define	STOCKAUTO_CONFIG_TRACE_CUTLOSS_PERCENT_DFT		0.7
struct STOCKAUTO_CONFIG
{
	STOCKAUTO_CONFIG_PYTHON		configPython;
	STOCKAUTO_CONFIG_DATA		configData;
	STOCKAUTO_CONFIG_TRACE		configTrace;
};

#endif // !__STOCK_CONFIG_DEF_H__
