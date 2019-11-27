/*stockSettingDialog.cpp
*/

#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtreewidget.h>
#include <qheaderview.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include "../include/qtObjectAgent.h"
#include "../include/qtStockAgent.h"
#include "../include/stockSettingDialog.h"

CStockSettingDilaog::CStockSettingDilaog(QWidget* parent, CQtObjectAgent* pStockAgent):QDialog(parent)
{
	m_pPythonSelectItem = NULL;
	m_pDataSelectItem = NULL;
	m_pTraceSelectItem = NULL;
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
	connect(m_pTreePython, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )), this, SLOT(OnPythonDoubleClicked(QTreeWidgetItem *, int)));
	connect(m_pTreePython, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(OnPythonChanged(QTreeWidgetItem *, int)));
	connect(m_pTreePython, SIGNAL(itemSelectionChanged()), this, SLOT(OnPythonSelectChanged()));

	connect(m_pTreeData, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnDataDoubleClicked(QTreeWidgetItem *, int)));
	connect(m_pTreeData, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(OnDataChanged(QTreeWidgetItem *, int)));
	connect(m_pTreeData, SIGNAL(itemSelectionChanged()), this, SLOT(OnDataSelectChanged()));

	connect(m_pTreeTrace, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnTraceDoubleClicked(QTreeWidgetItem *, int)));
	connect(m_pTreeTrace, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(OnTraceChanged(QTreeWidgetItem *, int)));
	connect(m_pTreeTrace, SIGNAL(itemSelectionChanged()), this, SLOT(OnTraceSelectChanged()));
}

void CStockSettingDilaog::InitPythonTreeWidget(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QStringList headerList;
	headerList << pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_PYTHON_NAME);
	headerList << "value";
	pTreeWidget->setHeaderLabels(headerList);
//	pTreeWidget->setColumnCount(2);
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
//	pTreeWidget->setColumnCount(2);
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
	headerList <<pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_NAME);
	headerList <<"value";
	pTreeWidget->setHeaderLabels(headerList);
//	pTreeWidget->setColumnCount(2);
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

void CStockSettingDilaog::OnPythonDoubleClicked(QTreeWidgetItem* pItem, int column)
{
	if (column == 1)
	{
		int index = m_pTreePython->indexOfTopLevelItem(pItem);
		if ((index == STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_MODULE_DIR) || (index == STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_UPDATE_DIR))
		{
			m_pTreePython->openPersistentEditor(pItem, 1);
			m_pPythonSelectItem = pItem;

			QString curDir = pItem->text(column);
			if (curDir.isEmpty())
			{
				curDir = QDir::currentPath();
			}

			QTextCodec* pCodec = QTextCodec::codecForLocale();
			QString selectDir = QFileDialog::getExistingDirectory(this, pCodec->toUnicode(STOCK_SETTING_DIALOG_SELECT_PYTHON_DIR), curDir, QFileDialog::DontResolveSymlinks);

			if (!selectDir.isEmpty())
			{
				pItem->setText(column, selectDir);
			}
		}
	}
}

void CStockSettingDilaog::OnDataDoubleClicked(QTreeWidgetItem* pItem, int column)
{
	if (column == 1)
	{
		int index = m_pTreeData->indexOfTopLevelItem(pItem);
		if ((index == STOCK_SETTING_DIALOG_EDIT_DATA_IDX_DATA_DIR))
		{
			m_pTreeData->openPersistentEditor(pItem, 1);
			m_pDataSelectItem = pItem;
		}
	}
}

void CStockSettingDilaog::OnTraceDoubleClicked(QTreeWidgetItem* pItem, int column)
{
	if (column == 1)
	{
		int index = m_pTreeTrace->indexOfTopLevelItem(pItem);
		if ((index <  STOCK_SETTING_DIALOG_EDIT_DTRACE_IDX_NUM))
		{
			m_pTreeData->openPersistentEditor(pItem, 1);
			m_pTraceSelectItem = pItem;
		}
	}
}

void CStockSettingDilaog::OnPythonChanged(QTreeWidgetItem* pItem, int column)
{
	if (column == 1)
	{
		int index = m_pTreePython->indexOfTopLevelItem(pItem);
		if ((index == STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_MODULE_DIR) || (index == STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_UPDATE_DIR))
		{
			QString text = pItem->text(column);
		}
	}
}

void CStockSettingDilaog::OnDataChanged(QTreeWidgetItem* pItem, int column)
{
	if (column == 1)
	{
		int index = m_pTreeData->indexOfTopLevelItem(pItem);
		if ((index == STOCK_SETTING_DIALOG_EDIT_DATA_IDX_DATA_DIR))
		{
			
		}
	}
}

void CStockSettingDilaog::OnTraceChanged(QTreeWidgetItem* pItem, int column)
{
	if (column == 1)
	{
		int index = m_pTreeTrace->indexOfTopLevelItem(pItem);
		if ((index <  STOCK_SETTING_DIALOG_EDIT_DTRACE_IDX_NUM))
		{
	
		}
	}
}

void CStockSettingDilaog::OnPythonSelectChanged()
{
	if (m_pPythonSelectItem != NULL)
		m_pTreePython->closePersistentEditor(m_pPythonSelectItem, 1);
}

void CStockSettingDilaog::OnDataSelectChanged()
{
	if (m_pDataSelectItem != NULL)
		m_pTreeData->closePersistentEditor(m_pDataSelectItem, 1);
}

void CStockSettingDilaog::OnTraceSelectChanged()
{
	if (m_pTraceSelectItem != NULL)
		m_pTreeTrace->closePersistentEditor(m_pTraceSelectItem, 1);
}