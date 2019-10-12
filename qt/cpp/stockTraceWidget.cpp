
#include <qlabel.h>
#include <qsplitter.h>
#include <qtextcodec.h>
#include <qtreewidget.h>
#include <qheaderview.h>
#include <qlayout.h>
#include <qwidget.h>
#include "../include/qtResourceDef.h"
#include "../include/stockTraceWidget.h"

CStockTraceWidget::CStockTraceWidget(QWidget *parent)
	: QWidget(parent)
{
	OnInit();

	Retranslate();
}

CStockTraceWidget::~CStockTraceWidget()
{
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
	char const* pWaitBuyHeaders[] = { STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_CODE_NAME ,STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_RAISE_BALANCE,
		STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_REACH_HIGH , STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_RSI , STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_BUY };

	for (int i = 0; i < STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_NUM; i++)
	{
		waitBuyHeaders << pCodec->toUnicode(pWaitBuyHeaders[i]);
	}
	m_pTreeWaitBuy->header()->setSectionResizeMode(QHeaderView::QHeaderView::Stretch);
	m_pTreeWaitBuy->header()->setStretchLastSection(false);
	m_pTreeWaitBuy->header()->setDefaultAlignment(Qt::AlignCenter);
	m_pTreeWaitBuy->setHeaderLabels(waitBuyHeaders);
	
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
}

void CStockTraceWidget::Retranslate()
{

}