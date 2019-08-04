/*stockConfigDef.h
*
*/

#ifndef __STOCK_CONFIG_DEF_H__
#define __STOCK_CONFIG_DEF_H__

#include <global.h>
#define STOCK_CONFIG_NAME_MAX		128

struct STOCKAUTO_CONFIG_CALC
{
	float fLimit;
};

/*Ĭ������*/
#define STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT		"E:\\work\\stock\\StockAuto\\src\\py"
#define	STOCKAUTO_CONFIG_PYTHON_KLINE_MODULE_DFT	PYTHON_MOUDLE_WEEK_KLINE
#define	STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR			"E:\\work\\stock\\StockAuto\\data"
#define	STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST		"stockList.db"
#define	STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE		"kline.db"

struct STOCKAUTO_CONFIG_PYTHON
{
	char moduleDir[STOCK_CONFIG_NAME_MAX];			/*python ģ��·��*/
	char klineModule[STOCK_CONFIG_NAME_MAX];		/*python K�߸���ģ������*/
	char updateDir[STOCK_CONFIG_NAME_MAX];			/*python ��ȡ���ݸ���·��*/
	char dbStockList[STOCK_CONFIG_NAME_MAX];		/*python ��ȡ��Ʊ�б����ݿ�����*/
	char dbStockKLine[STOCK_CONFIG_NAME_MAX];		/*python ��ȡ��ƱKLine���ݿ�����*/
};


struct STOCKAUTO_CONFIG
{
	STOCKAUTO_CONFIG_PYTHON		configPython;
};

#endif // !__STOCK_CONFIG_DEF_H__
