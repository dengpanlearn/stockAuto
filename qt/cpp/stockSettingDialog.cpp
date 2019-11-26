/*stockSettingDialog.cpp
*/

#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtreewidget.h>
#include <qheaderview.h>
#include <qtextcodec.h>
#include "../include/qtObjectAgent.h"
#include "../include/qtStockAgent.h"
#include "../include/stockSettingDialog.h"

CStockSettingDilaog::CStockSettingDilaog(QWidget* parent, CQtObjectAgent* pStockAgent):QDialog(parent)
{
	m_pStockAgent = pStockAgent;
	InitUi();
	RetranslateUi();
}

void CStockSettingDilaog::InitUi()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;

	STOCKAUTO_CONFIG_PYTHON configPython;
	pStockAgent->GetConfigPython(&configPython);

	m_pTreePython = new QTreeWidget();
	InitPythonTreeWidget(m_pTreePython, &configPython);

	STOCKAUTO_CONFIG_DATA configData;
	pStockAgent->GetConfigData(&configData);

	m_pTreeData = new QTreeWidget();
	InitDataTreeWidget(m_pTreeData, &configData);

	STOCKAUTO_CONFIG_TRACE configTrace;
	pStockAgent->GetConfigTrace(&configTrace);

	m_pTreeTrace = new QTreeWidget();
	InitTraceTreeWidget(m_pTreeTrace, &configTrace);
	
	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->addWidget(m_pTreePython);
	pLyt->addWidget(m_pTreeData);
	pLyt->addWidget(m_pTreeTrace);
	setLayout(pLyt);
}

void CStockSettingDilaog::RetranslateUi()
{

}

void CStockSettingDilaog::InitPythonTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QStringList headerList;
	headerList << pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_NAME);
	headerList << "value";
	pTreeWidget->setHeaderLabels(headerList);
	pTreeWidget->setColumnCount(2);
	pTreeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
	pTreeWidget->header()->setStretchLastSection(false);
	pTreeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
	pTreeWidget->setIndentation(0);
	QTreeWidgetItem* pItemLab = new QTreeWidgetItem();
	pItemLab->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_MODULE_DIR_NAME));
	pItemLab->setText(1, pCodec->toUnicode(pConfigPython->moduleDir));
	pTreeWidget->addTopLevelItem(pItemLab);

	pItemLab = new QTreeWidgetItem();
	pItemLab->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_KLINE_MODULE_NAME));
	pItemLab->setText(1, pCodec->toUnicode(pConfigPython->klineModule));
	pTreeWidget->addTopLevelItem(pItemLab);

	pItemLab = new QTreeWidgetItem();
	pItemLab->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_UPDATE_DIR_NAME));
	pItemLab->setText(1, pCodec->toUnicode(pConfigPython->updateDir));
	pTreeWidget->addTopLevelItem(pItemLab);

	pItemLab = new QTreeWidgetItem();
	pItemLab->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_DB_STOCKLIST_NAME));
	pItemLab->setText(1, pCodec->toUnicode(pConfigPython->dbStockList));
	pTreeWidget->addTopLevelItem(pItemLab);

	pItemLab = new QTreeWidgetItem();
	pItemLab->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_DB_STOCKKLINE_NAME));
	pItemLab->setText(1, pCodec->toUnicode(pConfigPython->dbStockKLine));
	pTreeWidget->addTopLevelItem(pItemLab);
}

void CStockSettingDilaog::InitDataTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_DATA* pConfigData)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QStringList headerList;
	headerList << pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_DATA_NAME);
	headerList << "value";
	pTreeWidget->setHeaderLabels(headerList);
	pTreeWidget->setColumnCount(2);
	pTreeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
	pTreeWidget->header()->setStretchLastSection(false);
	pTreeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
	pTreeWidget->setIndentation(0);

	QTreeWidgetItem* pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_DATA_DATA_DIR_NAME));
	pItem->setText(1, pCodec->toUnicode(pConfigData->dataDir));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_DATA_LIST_FILENAMEF_NAME));
	pItem->setText(1, pCodec->toUnicode(pConfigData->listFileName));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_DATA_KLINE_FILENAME_NAME));
	pItem->setText(1, pCodec->toUnicode(pConfigData->klineFileName));
	pTreeWidget->addTopLevelItem(pItem);


	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_DATA_TRACELOG_FILENAME_NAME));
	pItem->setText(1, pCodec->toUnicode(pConfigData->tracelogFileName));
	pTreeWidget->addTopLevelItem(pItem);
}

void CStockSettingDilaog::InitTraceTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QStringList headerList;
	headerList << pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_NAME);
	headerList << "value";
	pTreeWidget->setHeaderLabels(headerList);
	pTreeWidget->setColumnCount(2);
	pTreeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
	pTreeWidget->header()->setStretchLastSection(false);
	pTreeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
	pTreeWidget->setIndentation(0);

	QTreeWidgetItem* pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RAISE_PERCENT_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fRaisePercent, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RAISE_BALANCES_NAME));
	pItem->setText(1, QString::number(pConfigTrace->raiseBalances));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACEREACHHIGH_RANGES_NAME));
	pItem->setText(1, QString::number(pConfigTrace->reachHighRanges));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RSIBUY_WAITS_NAME));
	pItem->setText(1, QString::number(pConfigTrace->rsiBuyWaits));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RSIBUY_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fRsiBuy, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RSISELL_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fRsiSell, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RSISELL_WAITS_NAME));
	pItem->setText(1, QString::number(pConfigTrace->rsiSellWaits));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_CUTLOSS_PERCENT_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fCutLossPercent, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);
}
