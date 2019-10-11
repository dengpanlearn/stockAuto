
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
	pLabWaitBuy->setText(pCodec->toUnicode("STOCK_TRACE_WIDET_LAB_WAIT_BUY"));
	m_pTreeWaitBuy = new QTreeWidget();
	QVBoxLayout* pLytWaitBuy = new QVBoxLayout();
	pLytWaitBuy->addWidget(pLabWaitBuy);
	pLytWaitBuy->addWidget(m_pTreeWaitBuy);
	m_pWidgetWaitBuy->setLayout(pLytWaitBuy);

	QLabel* pLabWaitSell = new QLabel();
	pLabWaitSell->setText(pCodec->toUnicode("STOCK_TRACE_WIDET_LAB_WAIT_SELL"));
	m_pTreeWaitSell = new QTreeWidget();
	QVBoxLayout* pLytWaitSell = new QVBoxLayout();
	pLytWaitSell->addWidget(pLabWaitSell);
	pLytWaitSell->addWidget(m_pTreeWaitSell);
	m_pWidgetWaitSell->setLayout(pLytWaitSell);
}

void CStockTraceWidget::Retranslate()
{

}