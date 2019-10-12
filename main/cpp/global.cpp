/*
*global.cpp
*/

#include <stockAutoManager.h>
#include "../include/global.h"

CStockAutoManager g_autoManger;

BOOL GlobalInit()
{
	if (!g_autoManger.Create(STOCK_AUTO_MANAGER_TASK_NAME, STOCK_AUTO_MANAGER_TASK_SZ, STOCK_AUTO_MANAGER_TASK_PRI,
		0, STOCK_AUTO_MANAGER_TASK_TIMEOUT, STOCK_AUTO_MANAGER_TASK_EVENTS, STOCK_AUTO_MANAGER_TASK_EVENT_PARAM_SIZE))
	{
		printf("autoManger create fail\n");
		return FALSE;
	}

	return TRUE;
}

void GlobalExit()
{
	g_autoManger.StopWork();
	g_autoManger.WaitWorkExit(2500);
}
