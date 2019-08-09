/*stockConfigDef.h
*
*/

#ifndef __STOCK_CONFIG_DEF_H__
#define __STOCK_CONFIG_DEF_H__

#include <global.h>
#define STOCK_CONFIG_NAME_MAX		STOCK_AUTO_SAVE_DIR_MAX_SIZE

struct STOCKAUTO_CONFIG_CALC
{
	float fLimit;
};

/*Ĭ������*/
#define STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT		"E:\\work\\stock\\StockAuto\\src\\py"
#define	STOCKAUTO_CONFIG_PYTHON_KLINE_MODULE_DFT	PYTHON_MOUDLE_WEEK_KLINE
#define	STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR_DFT			"E:\\work\\stock\\StockAuto\\data"
#define	STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST_DFT		"stockList.db"
#define	STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE_DFT		"kline.db"

struct STOCKAUTO_CONFIG_PYTHON
{
	char moduleDir[STOCK_CONFIG_NAME_MAX];			/*python ģ��·��*/
	char klineModule[STOCK_CONFIG_NAME_MAX];		/*python K�߸���ģ������*/
	char updateDir[STOCK_CONFIG_NAME_MAX];			/*python ��ȡ���ݸ���·��*/
	char dbStockList[STOCK_CONFIG_NAME_MAX];		/*python ��ȡ��Ʊ�б����ݿ�����*/
	char dbStockKLine[STOCK_CONFIG_NAME_MAX];		/*python ��ȡ��ƱKLine���ݿ�����*/
};

#define STOCKAUTO_CONFIG_DATA_DATADIR_DFT			STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR_DFT
#define	STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT		STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST_DFT
#define	STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT	STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE_DFT

struct STOCKAUTO_CONFIG_DATA
{
	char dataDir[STOCK_CONFIG_NAME_MAX];
	char listFileName[STOCK_CONFIG_NAME_MAX];
	char klineFileName[STOCK_CONFIG_NAME_MAX];
};

struct STOCKAUTO_CONFIG
{
	STOCKAUTO_CONFIG_PYTHON		configPython;
	STOCKAUTO_CONFIG_DATA		configData
};

#endif // !__STOCK_CONFIG_DEF_H__
