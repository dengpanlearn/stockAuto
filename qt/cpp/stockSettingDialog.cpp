/*stockSettingDialog.cpp
*/

#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtreewidget.h>
#include <qheaderview.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qtGlobal.h>
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

	QTextCodec* pCodec = QTextCodec::codecForLocale();
	m_pBtnSave = new QPushButton(pCodec->toUnicode(STOCK_SETTING_DIALOG_BTN_SAVE));
	m_pBtnSave->setEnabled(false);
	m_pBtnCancle = new QPushButton(pCodec->toUnicode(STOCK_SETTING_DIALOG_BTN_CANCLE));

	QHBoxLayout* pLytBtn = new QHBoxLayout();
	pLytBtn->addWidget(m_pBtnSave);
	pLytBtn->addWidget(m_pBtnCancle);
	pLyt->addLayout(pLytBtn);
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

	connect(m_pBtnSave, SIGNAL(clicked(bool)), this, SLOT(OnSaveConfig(bool)));
	connect(m_pBtnCancle, SIGNAL(clicked(bool)), this, SLOT(OnCancleConfig(bool)));
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

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_DATA_TRACERECORD_FILENAME_NAME));
	pItem->setText(1, pCodec->toUnicode(pConfigData->traceRecordFileName));
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
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_REACHHIGH_RANGES_NAME));
	pItem->setText(1, QString::number(pConfigTrace->reachHighRanges));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_HIGHLOSS_PERCENT_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fHighLostPercent, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_RAISEBALANCE_WAITS_NAME));
	pItem->setText(1, QString::number(pConfigTrace->raiseBalanceWaits));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_PREVVOLUME_PERCENT_LOST_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fPrevVolumePercentLost, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_VOLUME_PERCENT_BUY_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fVolumePercentBuy, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_VOLUMEBUY_WAITS_NAME));
	pItem->setText(1, QString::number(pConfigTrace->volumeBuyWaits));
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

	pItem = new QTreeWidgetItem();
	pItem->setText(0, pCodec->toUnicode(STOCK_SETTING_DIALOG_EDIT_TRACE_CUTLOSS_AFTERTOP_NAME));
	pItem->setText(1, QString::number(pConfigTrace->fCutLossAfterTop, 'g', 4));
	pTreeWidget->addTopLevelItem(pItem);
}

void CStockSettingDilaog::GetPythonConfig(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_PYTHON* pConfigPython)
{
	memset(pConfigPython,0, sizeof(STOCKAUTO_CONFIG_PYTHON));
	QTreeWidgetItem* pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_MODULE_DIR);
	QString tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigPython->moduleDir);

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_KLINE_MODULE);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigPython->klineModule);
	
	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_UPDATE_DIR);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigPython->updateDir);

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_DB_STOCKLIST);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigPython->dbStockList);

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_DB_STOCKKLINE);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigPython->dbStockKLine);
}

void CStockSettingDilaog::GetDataConfig(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_DATA* pConfigData)
{
	memset(pConfigData, 0, sizeof(STOCKAUTO_CONFIG_DATA));
	QTreeWidgetItem* pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_DATA_IDX_DATA_DIR);
	QString tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigData->dataDir);

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_DATA_IDX_LIST_FILENAME);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigData->listFileName);

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_DATA_IDX_KLINE_FILENAME);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigData->klineFileName);

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_DATA_IDX_TRACELOG_FILENAME);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigData->tracelogFileName);


	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_DATA_IDX_TRACERECORD_FILENAME);
	tmpVal = pItem->text(1);
	QString2Char(tmpVal, pConfigData->traceRecordFileName);
}

void CStockSettingDilaog::GetTraceConfig(QTreeWidget* pTreeWidget, STOCKAUTO_CONFIG_TRACE* pConfigTrace)
{
	memset(pConfigTrace, 0, sizeof(STOCKAUTO_CONFIG_TRACE));
	QTreeWidgetItem* pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RAISE_PERCENT);
	QString tmpVal = pItem->text(1);
	pConfigTrace->fRaisePercent = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RAISE_BALANCES);
	tmpVal = pItem->text(1);
	pConfigTrace->raiseBalances = tmpVal.toInt();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_REACHHIGH_RANGES);
	tmpVal = pItem->text(1);
	pConfigTrace->reachHighRanges = tmpVal.toInt();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_HIGHLOSS_PERCENT);
	tmpVal = pItem->text(1);
	pConfigTrace->fHighLostPercent = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RAISEBALANCE_WAITS);
	tmpVal = pItem->text(1);
	pConfigTrace->raiseBalanceWaits = tmpVal.toInt();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_PREVVOLUMEPERCENT_LOST);
	tmpVal = pItem->text(1);
	pConfigTrace->fPrevVolumePercentLost = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_VOLUMEPERCENT_BUY);
	tmpVal = pItem->text(1);
	pConfigTrace->fVolumePercentBuy = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_VOLUMEBUY_WAITS);
	tmpVal = pItem->text(1);
	pConfigTrace->volumeBuyWaits = tmpVal.toInt();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSIBUY_WAITS);
	tmpVal = pItem->text(1);
	pConfigTrace->rsiBuyWaits = tmpVal.toInt();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSIBUY);
	tmpVal = pItem->text(1);
	pConfigTrace->fRsiBuy = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSISELL);
	tmpVal = pItem->text(1);
	pConfigTrace->fRsiSell = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSISELL_WAITS);
	tmpVal = pItem->text(1);
	pConfigTrace->rsiSellWaits = tmpVal.toInt();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_CUTLOSS_PERCENT);
	tmpVal = pItem->text(1);
	pConfigTrace->fCutLossPercent = tmpVal.toFloat();

	pItem = pTreeWidget->topLevelItem(STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_CUTLOSS_AFTERTOP);
	tmpVal = pItem->text(1);
	pConfigTrace->fCutLossAfterTop = tmpVal.toFloat();
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
			QString title = pCodec->toUnicode(STOCK_SETTING_DIALOG_SELECT_PYTHON_DIR);
			if (index == STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_UPDATE_DIR)
				title = pCodec->toUnicode(STOCK_SETTING_DIALOG_SELECT_DATA_DIR);
			QString selectDir = QFileDialog::getExistingDirectory(this, title, curDir, QFileDialog::DontResolveSymlinks);

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

			QString curDir = pItem->text(column);
			if (curDir.isEmpty())
			{
				curDir = QDir::currentPath();
			}

			QTextCodec* pCodec = QTextCodec::codecForLocale();
			QString selectDir = QFileDialog::getExistingDirectory(this, pCodec->toUnicode(STOCK_SETTING_DIALOG_SELECT_DATA_DIR), curDir, QFileDialog::DontResolveSymlinks);

			if (!selectDir.isEmpty())
			{
				pItem->setText(column, selectDir);
			}
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
			m_pTreeTrace->openPersistentEditor(pItem, 1);
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
			m_pBtnSave->setEnabled(true);
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
			m_pBtnSave->setEnabled(true);
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
			m_pBtnSave->setEnabled(true);
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

void CStockSettingDilaog::OnSaveConfig(bool check)
{
	STOCKAUTO_CONFIG_PYTHON python;
	STOCKAUTO_CONFIG_DATA   data;
	STOCKAUTO_CONFIG_TRACE	trace;
	GetPythonConfig(m_pTreePython, &python);
	GetDataConfig(m_pTreeData, &data);
	GetTraceConfig(m_pTreeTrace, &trace);
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;

	pStockAgent->UpdateConfigPython(&python);
	pStockAgent->UpdateConfigData(&data);
	pStockAgent->UpdateConfigTrace(&trace);
	pStockAgent->SyncConfig();
	close();
}

void CStockSettingDilaog::OnCancleConfig(bool check)
{
	close();
}