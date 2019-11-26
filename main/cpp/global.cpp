/*
*global.cpp
*/

#include <stockAutoManager.h>
#include <stockAutoWindow.h>
#include <stockConfigTask.h>
#include "../include/global.h"

CStockAutoManager g_autoManger;
CStockConfigTask g_configTask;
BOOL GlobalInit(CStockAutoWindow* pAutoWindow)
{
	if (!g_autoManger.Create(STOCK_AUTO_MANAGER_TASK_NAME, STOCK_AUTO_MANAGER_TASK_SZ, STOCK_AUTO_MANAGER_TASK_PRI,
		0, STOCK_AUTO_MANAGER_TASK_TIMEOUT, STOCK_AUTO_MANAGER_TASK_EVENTS, STOCK_AUTO_MANAGER_TASK_EVENT_PARAM_SIZE, pAutoWindow))
	{
		printf("autoManger create fail\n");
		return FALSE;
	}

	if (!g_configTask.Create(STOCK_CONFIG_TASK_NAME, STOCK_CONFIG_TASK_PRI, STOCK_CONFIG_TASK_SZ,
		0, STOCK_CONFIG_TASK_TIMEOUT, STOCK_CONFIG_FILE_NAME))
	{
		printf("config create fail\n");
		return FALSE;
	}

	return TRUE;
}

void GlobalExit()
{
	g_autoManger.StopWork();
	g_autoManger.WaitWorkExit(2500);

	g_configTask.StopWork();
	g_configTask.WaitWorkExit(2000);
}
