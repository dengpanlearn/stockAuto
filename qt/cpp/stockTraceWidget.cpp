
#include <qlabel.h>
#include <qsplitter.h>
#include <qtextcodec.h>
#include <qtreewidget.h>
#include <qtimer.h>
#include <qheaderview.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbrush.h>
#include <qwidget.h>
#include "../include/qtStockTraceDef.h"
#include "../include/qtStockAgent.h"
#include "../include/qtResourceDef.h"
#include "../include/stockTraceWidget.h"

#define	 STOCK_TRACE_WIDGET_SELL_CYCLE			1000
#define	 STOCK_TRACE_WIDGET_TRACE_BUF_COUNTS	20
CStockTraceWidget::CStockTraceWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent)
	: QWidget(parent)
{
	m_pExitAgent = pExitAgent;
	m_pStockAgent = pStockAgent;
	m_pTraceLogBuf = NULL;
	OnInit();
	Retranslate();
}

CStockTraceWidget::~CStockTraceWidget()
{
	if (m_pTraceLogBuf != NULL)
	{
		free(m_pTraceLogBuf);
		m_pTraceLogBuf = NULL;
	}
}


void CStockTraceWidget::OnInit()
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	m_pWidgetWaitBuy = new QWidget();
	m_pWidgetWaitBuy->setObjectName("Stock_Trace_Widget_Cell");

	m_pWidgetWaitSell = new QWidget();
	m_pWidgetWaitSell->setObjectName("Stock_Trace_Widget_Cell");

	QLabel* pLabWaitBuy = new QLabel();
	pLabWaitBuy->setText(pCodec->toUnicode(STOCK_TRACE_WIDGET_LAB_WAIT_BUY));
	pLabWaitBuy->setObjectName("Stock_Trace_Lab");
	pLabWaitBuy->setAlignment(Qt::AlignHCenter);
	m_pTreeWaitBuy = new QTreeWidget();
	QVBoxLayout* pLytWaitBuy = new QVBoxLayout();
	pLytWaitBuy->addWidget(pLabWaitBuy);
	pLytWaitBuy->addWidget(m_pTreeWaitBuy);
	m_pWidgetWaitBuy->setLayout(pLytWaitBuy);

	QLabel* pLabWaitSell = new QLabel();
	pLabWaitSell->setText(pCodec->toUnicode(STOCK_TRACE_WIDGET_LAB_WAIT_SELL));
	pLabWaitSell->setObjectName("Stock_Trace_Lab");
	pLabWaitSell->setAlignment(Qt::AlignHCenter);
	m_pTreeWaitSell = new QTreeWidget();
	QVBoxLayout* pLytWaitSell = new QVBoxLayout();
	pLytWaitSell->addWidget(pLabWaitSell);
	pLytWaitSell->addWidget(m_pTreeWaitSell);
	m_pWidgetWaitSell->setLayout(pLytWaitSell);

	m_pSplitter = new QSplitter(Qt::Vertical);
	m_pSplitter->addWidget(m_pWidgetWaitBuy);
	m_pSplitter->addWidget(m_pWidgetWaitSell);

	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->setMargin(0);
	pLyt->addWidget(m_pSplitter);
	this->setLayout(pLyt);
	QStringList waitBuyHeaders;
	char const* pWaitBuyHeaders[] = { STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_CODE_NAME, STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_REACH_HIGH ,
		 STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_RAISE_BALANCE, STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_VOLUME, STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_BUY };

	for (int i = 0; i < STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_NUM; i++)
	{
		waitBuyHeaders << pCodec->toUnicode(pWaitBuyHeaders[i]);
	}
	m_pTreeWaitBuy->header()->setSectionResizeMode(QHeaderView::QHeaderView::Stretch);
	m_pTreeWaitBuy->header()->setStretchLastSection(false);
	m_pTreeWaitBuy->header()->setDefaultAlignment(Qt::AlignCenter);
	m_pTreeWaitBuy->setHeaderLabels(waitBuyHeaders);
	m_pTreeWaitBuy->setIndentation(0);

	QStringList waitSellHeaders;
	char const* pWaitSellHeaders[] = { STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_CODE_NAME ,STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_MA10,
		STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_EARNS , STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_RSI , STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_CUT_BACK,
		STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_WAIT_SELL };

	for (int i = 0; i < STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_NUM; i++)
	{
		waitSellHeaders << pCodec->toUnicode(pWaitSellHeaders[i]);
	}

	m_pTreeWaitSell->header()->setSectionResizeMode(QHeaderView::QHeaderView::Stretch);
	
	m_pTreeWaitSell->header()->setStretchLastSection(false);
	m_pTreeWaitSell->header()->setDefaultAlignment(Qt::AlignCenter);
	m_pTreeWaitSell->setHeaderLabels(waitSellHeaders);
	m_pTreeWaitSell->setIndentation(0);

	m_pTimerSell = new QTimer(this);
	m_pTimerSell->setInterval(STOCK_TRACE_WIDGET_SELL_CYCLE);
	m_indxOfSellStockForUpdate = 0;
	m_pTraceLogBuf = (QT_STOCK_TRACE_LOG*)calloc(STOCK_TRACE_WIDGET_TRACE_BUF_COUNTS, sizeof(QT_STOCK_TRACE_LOG));
}

void CStockTraceWidget::Retranslate()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	connect(pStockAgent, SIGNAL(NotifyUiStockTrace()), this, SLOT(OnNotifyStockTrace()));
	connect(pStockAgent, SIGNAL(NotifyUiSellStatResponse()), this, SLOT(OnNotifyUiSellStatResponse()));
	connect(this, SIGNAL(SignalUpdateStockSellStat(QString& , QString& )), pStockAgent, SLOT(OnGetQuerySellStat(QString& , QString& )));
	connect(m_pTreeWaitBuy, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(OnSelectStock(QTreeWidgetItem*, int)));
	connect(m_pTreeWaitSell, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(OnSelectStock(QTreeWidgetItem*, int)));
	connect(m_pTimerSell, SIGNAL(timeout()), this, SLOT(OnUpdateSellStocks()));
	m_pTimerSell->start();
}

void CStockTraceWidget::OnNotifyStockTrace()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	QT_STOCK_TRACE_LOG* pBuf = m_pTraceLogBuf;
	if (pBuf == NULL)
		return;

	int bufCounts = pStockAgent->GetAckStockTrace(pBuf, STOCK_TRACE_WIDGET_TRACE_BUF_COUNTS);
	QT_STOCK_TRACE_LOG* pTraceLog = pBuf;
	while (bufCounts-- > 0)
	{
		if (pTraceLog->stat& QT_STOCK_TRACE_LOG_STAT_ADD)
		{
			AddTraceLogItem(pTraceLog->code, pTraceLog->stockName, pTraceLog->stat&0xff);
		}
		else if (pTraceLog->stat& QT_STOCK_TRACE_LOG_STAT_MODIFY)
		{
			ModifyTraceLogItem(pTraceLog->code, pTraceLog->stockName, pTraceLog->stat & 0xff);
		}
		else if(pTraceLog->stat& QT_STOCK_TRACE_LOG_STAT_DEL)
		{
			DeleteTraceLogItem(pTraceLog->code, pTraceLog->stockName);
		}
		else
		{

		}

		pTraceLog++;
	}
}

void CStockTraceWidget::OnSelectStock(QTreeWidgetItem * pItem, int column)
{
	QString codeName = pItem->text(STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_CODE_NAME);
	QStringList list = codeName.split(' ');
	if (list.size() < 2)
		return;
	emit SignalSelectStock(list[0], list[1]);
}

void CStockTraceWidget::AddTraceLogItem(char const* pCode, char const* pName, UINT stat)
{
	QTreeWidgetItem* pItem = new QTreeWidgetItem();
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QString codeName = pCodec->toUnicode(pCode)+" "+ pCodec->toUnicode(pName);
	pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_CODE_NAME, codeName);
	if (stat < QT_STOCK_TRACE_LOG_STAT_BUYED)
	{
		QLabel* pLabBack;
		m_pTreeWaitBuy->addTopLevelItem(pItem);
		switch (stat)
		{
		case QT_STOCK_TRACE_LOG_STAT_VOLUME_REACHED:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_WAIT_VOLUME, pLabBack);

		case QT_STOCK_TRACE_LOG_STAT_RAISE_BALANCED:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_RAISE_BALANCE, pLabBack);
	
		case QT_STOCK_TRACE_LOG_STAT_HIGH_REACHED:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_REACH_HIGH, pLabBack);

		}
	}
	else if (stat == QT_STOCK_TRACE_LOG_STAT_BUYED)
	{
		pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_MA10, "--/--");
		pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_EARNS, "--/--/--");
		pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_RSI, "--");
		pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_CUT_BACK, "--/--/--");
		m_pTreeWaitSell->addTopLevelItem(pItem);
	}
	else
	{
		delete pItem;
	}
}

void CStockTraceWidget::ModifyTraceLogItem(char const* pCode, char const* pName, UINT stat)
{
	if (stat < QT_STOCK_TRACE_LOG_STAT_BUYED)
	{
		QTreeWidgetItem* pItem = FindTraceLogItem(m_pTreeWaitBuy, pCode, pName);
		if (pItem == NULL)
			return;

		QLabel* pLabBack;
		switch (stat)
		{
		case QT_STOCK_TRACE_LOG_STAT_BUYING:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_WAIT_BUY, pLabBack);
			break;

		case QT_STOCK_TRACE_LOG_STAT_VOLUME_REACHED:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_WAIT_VOLUME, pLabBack);
			break;

		case QT_STOCK_TRACE_LOG_STAT_RAISE_BALANCED:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_RAISE_BALANCE, pLabBack);
			break;

		case QT_STOCK_TRACE_LOG_STAT_HIGH_REACHED:
			pLabBack = new QLabel();
			m_pTreeWaitBuy->setItemWidget(pItem, STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_REACH_HIGH, pLabBack);
			break;
		}
	}
	else if (stat == QT_STOCK_TRACE_LOG_STAT_BUYED)
	{
		QTreeWidgetItem* pItem = FindTraceLogItem(m_pTreeWaitBuy, pCode, pName);
		if (pItem != NULL)
		{
			int index = m_pTreeWaitBuy->indexOfTopLevelItem(pItem);
			m_pTreeWaitBuy->takeTopLevelItem(index);
			delete pItem;
		}

		AddTraceLogItem(pCode, pName, stat);
	}
	else if (stat == QT_STOCK_TRACE_LOG_STAT_SELLING)
	{
		// 待加入买入、卖出接口后添加该功能
	}
	else if (stat == QT_STOCK_TRACE_LOG_STAT_SELLED)
	{
		QTreeWidgetItem* pItem = FindTraceLogItem(m_pTreeWaitSell, pCode, pName);
		if (pItem != NULL)
		{
			int index = m_pTreeWaitSell->indexOfTopLevelItem(pItem);
			m_pTreeWaitSell->takeTopLevelItem(index);
			delete pItem;
		}
	}
}

void CStockTraceWidget::DeleteTraceLogItem(char const* pCode, char const* pName)
{
	QTreeWidgetItem* pItem = FindTraceLogItem(m_pTreeWaitBuy, pCode, pName);
	if (pItem != NULL)
	{
		int index = m_pTreeWaitBuy->indexOfTopLevelItem(pItem);
		m_pTreeWaitBuy->takeTopLevelItem(index);
		delete pItem;

		return;
	}
	
	pItem = FindTraceLogItem(m_pTreeWaitSell, pCode, pName);
	if (pItem != NULL)
	{
		int index = m_pTreeWaitSell->indexOfTopLevelItem(pItem);
		m_pTreeWaitSell->takeTopLevelItem(index);
		delete pItem;
	}
}

QTreeWidgetItem* CStockTraceWidget::FindTraceLogItem(QTreeWidget* pTreeWidget, char const* pCode, char const* pName)
{
	int counts = pTreeWidget->topLevelItemCount();

	for (int i = 0; i < counts; i++)
	{
		QTreeWidgetItem* pItem = pTreeWidget->topLevelItem(i);
		QString codeName = pItem->text(STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_CODE_NAME);

		QTextCodec* pCodec = QTextCodec::codecForLocale();
		QString findCodeName = pCodec->toUnicode(pCode) + " " + pCodec->toUnicode(pName);
		if (findCodeName == codeName)
		{
			return pItem;
		}
	}

	return NULL;
}

void CStockTraceWidget::OnUpdateSellStocks()
{
	int stockCnts = m_pTreeWaitSell->topLevelItemCount();
	if (stockCnts == 0)
		return;

	if (m_indxOfSellStockForUpdate >= stockCnts)
		m_indxOfSellStockForUpdate = 0;

	QTreeWidgetItem* pItem = m_pTreeWaitSell->topLevelItem(m_indxOfSellStockForUpdate);

	QString codeName = pItem->text(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_CODE_NAME);
	QStringList list = codeName.split(' ');
	if (list.size() >= 2)
		emit SignalUpdateStockSellStat(list[0], list[1]);

	m_indxOfSellStockForUpdate++;
}

void CStockTraceWidget::OnNotifyUiSellStatResponse()
{
	QT_STOCK_SELLSTAT_INFO_EXE stockSellStat;
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	if (!pStockAgent->GetStockSellStat(&stockSellStat))
		return;

	if (stockSellStat.traceStep != CALC_STOCK_TRADE_STEP_WAIT_SELL)
		return;

	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QString stockCodeName = pCodec->toUnicode(stockSellStat.code) + " " + pCodec->toUnicode(stockSellStat.name);
	int stockCnts = m_pTreeWaitSell->topLevelItemCount();

	for (int i = 0; i < stockCnts; i++)
	{
		QTreeWidgetItem* pItem = m_pTreeWaitSell->topLevelItem(i);
		QString codeName = pItem->text(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_CODE_NAME);

		if (codeName == stockCodeName)
		{
			QString ma10 = QString("%1/%2").arg(stockSellStat.fMa).arg(stockSellStat.fCurVal);

			pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_MA10, ma10);

			float fCutLoss = stockSellStat.fCurVal - stockSellStat.fBuyVal;
			float percent = fCutLoss * 100 / stockSellStat.fBuyVal;
			QString cur = QString("%1/%2/%3").arg(stockSellStat.fBuyVal).arg(stockSellStat.fCurVal).arg(percent);
			pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_EARNS, cur);

		
			pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_RSI, QString::number(stockSellStat.fRsi, 'g', 4));

			fCutLoss = stockSellStat.fTopVal - stockSellStat.fCurVal;
			percent = fCutLoss * 100 / stockSellStat.fTopVal;
			QString cut = QString("%1/%2/%3").arg(stockSellStat.fTopVal).arg(stockSellStat.fCurVal).arg(percent);
			pItem->setText(STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_CUT_BACK, cut);
			break;
		}
	}
}