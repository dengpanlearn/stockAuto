/*
* stockConfig.h
*/

#ifndef __STOCK_CONFIG_H__
#define	__STOCK_CONFIG_H__

#include <dp.h>
#include "stockConfigDef.h"

class QSettings;
class CStockConfig
{
public:
	CStockConfig();
	~CStockConfig();

public:
	BOOL LoadConfig(const char* pFileName);

	inline STOCKAUTO_CONFIG_PYTHON const* GetConfigPythonPtr()const;
	inline STOCKAUTO_CONFIG_DATA const* GetConfigDataPtr()const;
	inline STOCKAUTO_CONFIG_TRACE const* GetConfigTracePtr()const;

	void SaveConfig(const char* pFileName);

	void UpdateConfigPython(STOCKAUTO_CONFIG_PYTHON const* pConfigPython);
	void UpdateConfigData(STOCKAUTO_CONFIG_DATA const* pConfigData);
	void UpdateConfigTrace(STOCKAUTO_CONFIG_TRACE const* pConfigTrace);

protected:
	void OnInit();

	void LoadConfigPython(QSettings* pConfigSettings, STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void LoadConfigData(QSettings* pConfigSettings, STOCKAUTO_CONFIG_DATA* pConfigData);
	void LoadConfigTrace(QSettings* pConfigSettings, STOCKAUTO_CONFIG_TRACE* pConfigTrace);

	void SaveConfigPython(QSettings* pConfigSettings, STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void SaveConfigData(QSettings* pConfigSettings, STOCKAUTO_CONFIG_DATA* pConfigData);
	void SaveConfigTrace(QSettings* pConfigSettings, STOCKAUTO_CONFIG_TRACE* pConfigTrace);

private:
	STOCKAUTO_CONFIG	m_stockConfig;
};

inline STOCKAUTO_CONFIG_PYTHON const* CStockConfig::GetConfigPythonPtr()const
{
	return &m_stockConfig.configPython;
}

inline STOCKAUTO_CONFIG_DATA const* CStockConfig::GetConfigDataPtr()const
{
	return &m_stockConfig.configData;
}

inline STOCKAUTO_CONFIG_TRACE const* CStockConfig::GetConfigTracePtr()const
{
	return &m_stockConfig.configTrace;
}

#endif /*__STOCK_CONFIG_H__*/