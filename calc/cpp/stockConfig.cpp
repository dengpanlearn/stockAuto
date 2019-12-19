/*stockConfig.cpp
*
*/

#include <QtCore/qtextcodec.h>
#include <QtCore/qsettings.h>
#include <qtGlobal.h>
#include "../include/stockConfig.h"

CStockConfig::CStockConfig()
{
	memset(&m_stockConfig, 0, sizeof(m_stockConfig));
	OnInit();
}

CStockConfig::~CStockConfig()
{

}

BOOL CStockConfig::LoadConfig(const char* pFileName)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QSettings configSetting(pCodec->toUnicode(pFileName), QSettings::IniFormat);

	QStringList groups = configSetting.childGroups();
	if (groups.contains(pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_NAME)))
	{
		LoadConfigPython(&configSetting, &m_stockConfig.configPython);
	}

	if (groups.contains(pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_NAME)))
	{
		LoadConfigData(&configSetting, &m_stockConfig.configData);
	}

	if (groups.contains(pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_NAME)))
	{
		LoadConfigTrace(&configSetting, &m_stockConfig.configTrace);
	}

	return TRUE;
}

void CStockConfig::SaveConfig(const char* pFileName)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QSettings configSetting(pCodec->toUnicode(pFileName), QSettings::IniFormat);
	configSetting.clear();

	SaveConfigPython(&configSetting, &m_stockConfig.configPython);
	SaveConfigData(&configSetting, &m_stockConfig.configData);
	SaveConfigTrace(&configSetting, &m_stockConfig.configTrace);
	configSetting.sync();
}

void CStockConfig::UpdateConfigPython(STOCKAUTO_CONFIG_PYTHON const* pConfigPython)
{
	memcpy(&m_stockConfig.configPython, pConfigPython, sizeof(STOCKAUTO_CONFIG_PYTHON));
}

void CStockConfig::UpdateConfigData(STOCKAUTO_CONFIG_DATA const* pConfigData)
{
	memcpy(&m_stockConfig.configData, pConfigData, sizeof(STOCKAUTO_CONFIG_DATA));
}

void CStockConfig::UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace)
{
	memcpy(&m_stockConfig.configTrace, pConfigTrace, sizeof(STOCKAUTO_CONFIG_TRACE));
}


void CStockConfig::OnInit()
{
	STOCKAUTO_CONFIG_PYTHON* pConfigPython = &m_stockConfig.configPython;
	strncpy(pConfigPython->moduleDir, STOCKAUTO_CONFIG_PYTHON_MODULEDIR_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigPython->klineModule, STOCKAUTO_CONFIG_PYTHON_KLINE_MODULE_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigPython->updateDir, STOCKAUTO_CONFIG_PYTHON_UPDATE_DIR_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigPython->dbStockList, STOCKAUTO_CONFIG_PYTHON_DB_STOCKLIST_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigPython->dbStockKLine, STOCKAUTO_CONFIG_PYTHON_DB_STOCKKLINE_DFT, STOCK_CONFIG_NAME_MAX);

	STOCKAUTO_CONFIG_DATA* pConfigData = &m_stockConfig.configData;
	strncpy(pConfigData->dataDir, STOCKAUTO_CONFIG_DATA_DATADIR_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigData->listFileName, STOCKAUTO_CONFIG_DATA_LIST_FILENAME_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigData->klineFileName, STOCKAUTO_CONFIG_DATA_KLINE_FILENAME_DFT, STOCK_CONFIG_NAME_MAX);
	strncpy(pConfigData->tracelogFileName, STOCKAUTO_CONFIG_DATA_TRACELOG_FILENAME_DFT, STOCK_CONFIG_NAME_MAX);

	STOCKAUTO_CONFIG_TRACE* pConfigTrace = &m_stockConfig.configTrace;
	pConfigTrace->fRaisePercent = STOCKAUTO_CONFIG_TRACE_RAISE_PERCENT_DFT;
	pConfigTrace->raiseBalances = STOCKAUTO_CONFIG_TRACE_RAISE_BALANCES_DFT;
	pConfigTrace->reachHighRanges = STOCKAUTO_CONFIG_TRACE_REAHHIGH_RANGES_DFT;
	pConfigTrace->rsiBuyWaits = STOCKAUTO_CONFIG_TRACE_RSI_BUY_WAIT_DFT;
	pConfigTrace->fRsiBuy = STOCKAUTO_CONFIG_TRACE_RSI_BUY_DFT;
	pConfigTrace->fRsiSell = STOCKAUTO_CONFIG_TRACE_RSI_SELL_DFT;
	pConfigTrace->rsiSellWaits = STOCKAUTO_CONFIG_TRACE_RSI_SELL_WAITS_DFT;
	pConfigTrace->fCutLossPercent = STOCKAUTO_CONFIG_TRACE_CUTLOSS_PERCENT_DFT;
	pConfigTrace->fCutLossAfterTop = STOCKAUTO_CONFIG_TRACE_CUTLOSS_AFTERTOP_DFT;

}

void CStockConfig::LoadConfigPython(QSettings* pConfigSettings, STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	pConfigSettings->beginGroup(pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_NAME));
	QStringList keys = pConfigSettings->allKeys();

	QString tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_MODULE_DIR);

	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigPython->moduleDir, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigPython->moduleDir);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_KLINE_MODULE);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigPython->klineModule, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigPython->klineModule);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_UPDATE_DIR);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigPython->updateDir, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigPython->updateDir);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_DB_STOCKLIST);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigPython->dbStockList, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigPython->dbStockList);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_DB_STOCKKLINE);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigPython->dbStockKLine, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigPython->dbStockKLine);
	}

	pConfigSettings->endGroup();
}

void CStockConfig::LoadConfigData(QSettings* pConfigSettings, STOCKAUTO_CONFIG_DATA* pConfigData)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	pConfigSettings->beginGroup(pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_NAME));
	QStringList keys = pConfigSettings->allKeys();

	QString tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_DATA_DIR);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigData->dataDir, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigData->dataDir);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_STOCKLIST_FILE);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigData->listFileName, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigData->listFileName);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_STOCKKLINE_FILE);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigData->klineFileName, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigData->klineFileName);
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_TRACELOG_FILE);
	if (keys.contains(tmpKey))
	{
		QString val = pConfigSettings->value(tmpKey).toString();
		memset(pConfigData->tracelogFileName, 0, STOCK_CONFIG_NAME_MAX);
		QString2Char(val, pConfigData->tracelogFileName);
	}

	pConfigSettings->endGroup();
}

void CStockConfig::LoadConfigTrace(QSettings* pConfigSettings, STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	pConfigSettings->beginGroup(pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_NAME));
	QStringList keys = pConfigSettings->allKeys();

	QString tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_RAISE_PERCENT);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->fRaisePercent = pConfigSettings->value(tmpKey).toFloat();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_RAISE_BALANCES);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->raiseBalances = pConfigSettings->value(tmpKey).toInt();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_HIGH_RANGES);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->reachHighRanges = pConfigSettings->value(tmpKey).toInt();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_BUYWAITS);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->rsiBuyWaits = pConfigSettings->value(tmpKey).toInt();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_BUY);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->fRsiBuy = pConfigSettings->value(tmpKey).toFloat();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_SELL);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->fRsiSell = pConfigSettings->value(tmpKey).toFloat();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_SELLWAITS);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->rsiSellWaits = pConfigSettings->value(tmpKey).toInt();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_CUTLOSS_PERCENT);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->fCutLossPercent = pConfigSettings->value(tmpKey).toFloat();
	}

	tmpKey = pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_CUTLOSS_AFTERTOP);
	if (keys.contains(tmpKey))
	{
		pConfigTrace->fCutLossAfterTop = pConfigSettings->value(tmpKey).toFloat();
	}

	pConfigSettings->endGroup();
}


void CStockConfig::SaveConfigPython(QSettings* pConfigSettings, STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	pConfigSettings->beginGroup(pCodec->toUnicode(STOCKAUTO_CONFIG_PYTHON_GROUP_NAME));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_PYTHON_GROUP_MODULE_DIR, pCodec->toUnicode(pConfigPython->moduleDir));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_PYTHON_GROUP_KLINE_MODULE, pCodec->toUnicode(pConfigPython->klineModule));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_PYTHON_GROUP_UPDATE_DIR, pCodec->toUnicode(pConfigPython->updateDir));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_PYTHON_GROUP_DB_STOCKLIST, pCodec->toUnicode(pConfigPython->dbStockList));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_PYTHON_GROUP_DB_STOCKKLINE, pCodec->toUnicode(pConfigPython->dbStockKLine));
	pConfigSettings->endGroup();
}

void CStockConfig::SaveConfigData(QSettings* pConfigSettings, STOCKAUTO_CONFIG_DATA* pConfigData)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	pConfigSettings->beginGroup(pCodec->toUnicode(STOCKAUTO_CONFIG_DATA_GROUP_NAME));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_DATA_GROUP_DATA_DIR, pCodec->toUnicode(pConfigData->dataDir));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_DATA_GROUP_STOCKLIST_FILE, pCodec->toUnicode(pConfigData->listFileName));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_DATA_GROUP_STOCKKLINE_FILE, pCodec->toUnicode(pConfigData->klineFileName));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_DATA_GROUP_TRACELOG_FILE, pCodec->toUnicode(pConfigData->tracelogFileName));
	pConfigSettings->endGroup();
}

void CStockConfig::SaveConfigTrace(QSettings* pConfigSettings, STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	pConfigSettings->beginGroup(pCodec->toUnicode(STOCKAUTO_CONFIG_TRACE_GROUP_NAME));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_RAISE_PERCENT, QString::number(pConfigTrace->fRaisePercent, 'g', 4));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_RAISE_BALANCES, QString::number(pConfigTrace->raiseBalances));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_HIGH_RANGES, QString::number(pConfigTrace->reachHighRanges));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_BUYWAITS, QString::number(pConfigTrace->rsiBuyWaits));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_BUY, QString::number(pConfigTrace->fRsiBuy,'g', 4));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_SELL, QString::number(pConfigTrace->fRsiSell,'g', 4));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_RSI_SELLWAITS, QString::number(pConfigTrace->rsiSellWaits));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_CUTLOSS_PERCENT, QString::number(pConfigTrace->fCutLossPercent,'g', 4));
	pConfigSettings->setValue(STOCKAUTO_CONFIG_TRACE_GROUP_CUTLOSS_AFTERTOP, QString::number(pConfigTrace->fCutLossAfterTop, 'g', 4));
	pConfigSettings->endGroup();
}