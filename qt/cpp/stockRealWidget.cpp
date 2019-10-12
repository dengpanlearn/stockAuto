/*
*stockRealWidget.cpp
*/

#include <qtextedit.h>
#include <qtextcodec.h>
#include <qlayout.h>
#include <qlabel.h>
#include "../include/stockRealWidget.h"

CStockRealWidget::CStockRealWidget(QWidget *parent)
	: QWidget(parent)
{
	OnInit();
}

CStockRealWidget::~CStockRealWidget()
{
}


void CStockRealWidget::OnInit()
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	for (int i = 0; i < STOCK_REAL_WIDGET_IDEX_NUM; i++)
	{
		m_pLabCode[i] = new QLabel();
		m_pLabCode[i]->setText(pCodec->toUnicode(STOCK_REAL_WIDGET_TEXT_DFT));
		m_pLabCode[i]->setObjectName("Stock_Real_Lab_Val");
	}
	 

	QLabel* pLabClose = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_CLOSE));
	QLabel* pLabOpen = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_OPEN));
	QLabel* pLabHigh = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_HIGH));
	QLabel* pLabLow = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_LOW));
	QLabel* pLabMA10 = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_MA10));
	QLabel* pLabRSI7 = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_RSI7));

	QHBoxLayout* pLytCode = new QHBoxLayout();
	pLytCode->setAlignment(Qt::AlignHCenter);
	pLytCode->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_CODE]);
	pLytCode->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_NAME]);
	m_pLabCode[STOCK_REAL_WIDGET_IDEX_CODE]->setObjectName("Stock_Real_Lab_Code");
	m_pLabCode[STOCK_REAL_WIDGET_IDEX_NAME]->setObjectName("Stock_Real_Lab_Code");

	QHBoxLayout* pLytClose = new QHBoxLayout();
	pLytClose->addWidget(pLabClose);
	pLytClose->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_CLOSE]);

	QHBoxLayout* pLytOpen = new QHBoxLayout();
	pLytOpen->addWidget(pLabOpen);
	pLytOpen->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_OPEN]);

	QHBoxLayout* pLytHigh = new QHBoxLayout();
	pLytHigh->addWidget(pLabHigh);
	pLytHigh->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_HIGH]);

	QHBoxLayout* pLytLow = new QHBoxLayout();
	pLytLow->addWidget(pLabLow);
	pLytLow->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_LOW]);

	QHBoxLayout* pLytMA10 = new QHBoxLayout();
	pLytMA10->addWidget(pLabMA10);
	pLytMA10->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_MA10]);

	QHBoxLayout* pLytRSI7 = new QHBoxLayout();
	pLytRSI7->addWidget(pLabRSI7);
	pLytRSI7->addWidget(m_pLabCode[STOCK_REAL_WIDGET_IDEX_RSI7]);

	QVBoxLayout* pLytMain = new QVBoxLayout();
	pLytMain->setAlignment(Qt::AlignTop);
	QLabel* pSeperator = new QLabel();
	pSeperator->setObjectName("Stock_Real_Lab_Sep");
	pLytMain->addLayout(pLytCode);
	pLytMain->addWidget(pSeperator);
	pLytMain->addLayout(pLytClose);
	pLytMain->addLayout(pLytOpen);
	pLytMain->addLayout(pLytHigh);
	pLytMain->addLayout(pLytLow);
	pLytMain->addLayout(pLytMA10);
	pLytMain->addLayout(pLytRSI7);
	pLytMain->setMargin(0);
	this->setLayout(pLytMain);
	this->setObjectName("Stock_Real_Widget");
}

void CStockRealWidget::Retranslate()
{

}