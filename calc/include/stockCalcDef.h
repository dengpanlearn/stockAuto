/*
*stockCalcDef.h
*/

#ifndef __STOCK_CALC_DEF_H__
#define __STOCK_CALC_DEF_H__

#include <taskDef.h>
#define	STOCK_CODE_NAME_MAX				32
#define	STOCK_AUTO_COUNTS_MAX			3700
struct STOCK_CODE_NAME
{
	char	code[STOCK_CODE_NAME_MAX];
	char	name[STOCK_CODE_NAME_MAX];
};

struct STOCK_MANAGER_JOB_LIST
{
	UINT    jobStep;
	UINT	stockCounts;
	STOCK_CODE_NAME	codeName[0];
};

enum STOCK_CALC_EVENT_CMD
{
	STOCK_CALC_EVENT_UPDATE_STOCK_LIST = 0x00000001,
	STOCK_CALC_EVENT_UPDATE_STOCK_KLINE = 0x00000002,
};

typedef TASK_EVENT_PARAM	STOCK_CALC_UPDATE_LIST;



struct STOCK_CALC_UPDATE_KLINE
{
	TASK_EVENT_PARAM	eventParam;
	char	codeName[STOCK_CODE_NAME_MAX];
	int		updateCycles;
};
#endif // !__STOCK_CALC_DEF_H__
