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
class QTreeWidgetItem;
class QPushButton;

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

	void GetPythonConfig(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_PYTHON* pConfigPython);
	void GetDataConfig(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_DATA* pConfigData);
	void GetTraceConfig(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_TRACE* pConfigTrace);

private slots:
	void OnPythonDoubleClicked(QTreeWidgetItem* pItem, int column);
	void OnDataDoubleClicked(QTreeWidgetItem* pItem, int column);
	void OnTraceDoubleClicked(QTreeWidgetItem* pItem, int column);

	void OnPythonChanged(QTreeWidgetItem* pItem, int column);
	void OnDataChanged(QTreeWidgetItem* pItem, int column);
	void OnTraceChanged(QTreeWidgetItem* pItem, int column);

	void OnPythonSelectChanged();
	void OnDataSelectChanged();
	void OnTraceSelectChanged();

	void OnSaveConfig(bool check);
	void OnCancleConfig(bool check);
private:
	QTreeWidget* m_pTreePython;
	QTreeWidget* m_pTreeData;
	QTreeWidget* m_pTreeTrace;

	QTreeWidgetItem* m_pPythonSelectItem;
	QTreeWidgetItem* m_pDataSelectItem;
	QTreeWidgetItem* m_pTraceSelectItem;
	CQtObjectAgent*	m_pStockAgent;

	QPushButton* m_pBtnSave;
	QPushButton* m_pBtnCancle;
};
#endif // !__STOCK_SETTING_H__


