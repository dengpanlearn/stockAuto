/*
*stockSettingDialog.\h
*/


#ifndef __STOCK_SETTING_DIALOG_H__
#define __STOCK_SETTING_DIALOG_H__

#include <qdialog.h>
#include "qtResourceDef.h"
class QLineEdit;
class QTreeWidget;
class CQtObjectAgent;

struct STOCKAUTO_CONFIG_PYTHON;
struct STOCKAUTO_CONFIG_DATA;
struct STOCKAUTO_CONFIG_TRACE;

class CStockSettingDilaog : public QDialog
{
	Q_OBJECT
public:
	CStockSettingDilaog(QWidget* parent, CQtObjectAgent* pStockAgent);
	~CStockSettingDilaog() {}

private:
	void InitUi();
	void RetranslateUi();

	void InitPythonTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void InitDataTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_DATA* pConfigData);
	void InitTraceTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_TRACE* pConfigTrace);
private:
	QTreeWidget* m_pTreePython;
	QTreeWidget* m_pTreeData;
	QTreeWidget* m_pTreeTrace;
	CQtObjectAgent*	m_pStockAgent;
};
#endif // !__STOCK_SETTING_H__


