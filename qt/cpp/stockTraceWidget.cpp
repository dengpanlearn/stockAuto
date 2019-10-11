
#include <qlabel.h>
#include <qsplitter.h>
#include <qtextcodec.h>
#include <qtreewidget.h>
#include <qlayout.h>
#include <qwidget.h>
#include "../include/qtResourceDef.h"
#include "stockTraceWidget.h"

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
	m_pTreeWaitBuy = new QTreeWidget();
	QVBoxLayout* pLytWaitBuy = new QVBoxLayout();
	pLytWaitBuy->setAlignment(Qt::AlignCenter);
	pLytWaitBuy->addWidget(pLabWaitBuy);
	pLytWaitBuy->addWidget(m_pTreeWaitBuy);
	m_pWidgetWaitBuy->setLayout(pLytWaitBuy);

	QLabel* pLabWaitSell = new QLabel();
	pLabWaitSell->setText(pCodec->toUnicode(STOCK_TRACE_WIDGET_LAB_WAIT_SELL));
	pLabWaitSell->setObjectName("Stock_Trace_Lab");
	m_pTreeWaitSell = new QTreeWidget();
	QVBoxLayout* pLytWaitSell = new QVBoxLayout();
	pLytWaitSell->setAlignment(Qt::AlignCenter);
	pLytWaitSell->addWidget(pLabWaitSell);
	pLytWaitSell->addWidget(m_pTreeWaitSell);
	m_pWidgetWaitSell->setLayout(pLytWaitSell);

	m_pSplitter = new QSplitter(Qt::Vertical);
	m_pSplitter->addWidget(m_pWidgetWaitBuy);
	m_pSplitter->addWidget(m_pWidgetWaitSell);
	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->addWidget(m_pSplitter);
	this->setLayout(pLyt);
	QStringList waitBuyHeaders;
	char const* pWaitBuyHeaders[] = { STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_CODE_NAME ,STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_RAISE_BALANCE,
		STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_REACH_HIGH , STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_RSI , STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_BUY };

	for (int i = 0; i < STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_NUM; i++)
	{
		waitBuyHeaders << pCodec->toUnicode(pWaitBuyHeaders[i]);
	}
	m_pTreeWaitBuy->setHeaderLabels(waitBuyHeaders);

	QStringList waitSellHeaders;
	char const* pWaitSellHeaders[] = { STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_CODE_NAME ,STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_MA10,
		STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_EARNS , STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_RSI , STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_CUT_BACK,
		STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_WAIT_SELL };

	for (int i = 0; i < STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_NUM; i++)
	{
		waitSellHeaders << pCodec->toUnicode(pWaitSellHeaders[i]);
	}

	m_pTreeWaitSell->setHeaderLabels(waitSellHeaders);
}

void CStockTraceWidget::Retranslate()
{

}