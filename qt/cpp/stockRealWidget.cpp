/*
*stockRealWidget.cpp
*/

#include <qtextedit.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qtStockAgent.h>
#include "../include/stockRealWidget.h"

#define	STOCK_REAL_WIDGET_FRESH_TIMEOUT		1000

CStockRealWidget::CStockRealWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent)
	: QWidget(parent)
{
	m_pExitAgent = pExitAgent;
	m_pStockAgent = pStockAgent;
	OnInit();
	Retranslate();
}

CStockRealWidget::~CStockRealWidget()
{
}


void CStockRealWidget::OnInit()
{
	QWidget* pRealWidget = new QWidget();
	QWidget* pTraceWidget = new QWidget();
	m_pSpliter = new QSplitter(Qt::Vertical);
	m_pSpliter->addWidget(pRealWidget);
	m_pSpliter->addWidget(pTraceWidget);

	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->setMargin(0);
	pLyt->addWidget(m_pSpliter);
	this->setLayout(pLyt);
	OnInitRealWidget(pRealWidget);
	OnInitTraceWidget(pTraceWidget);
	m_pTimer = new QTimer(this);
	m_pTimer->setInterval(STOCK_REAL_WIDGET_FRESH_TIMEOUT);
	m_pTimer->start();
}

void CStockRealWidget::OnInitRealWidget(QWidget* pWidget)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	for (int i = 0; i < STOCK_REAL_WIDGET_INDEX_NUM; i++)
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
	QLabel* pLabData = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_LAB_DATE));
	QHBoxLayout* pLytCode = new QHBoxLayout();
	pLytCode->setAlignment(Qt::AlignHCenter);
	pLytCode->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_CODE]);
	pLytCode->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_NAME]);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_CODE]->setObjectName("Stock_Real_Lab_Code");
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_NAME]->setObjectName("Stock_Real_Lab_Code");

	QHBoxLayout* pLytClose = new QHBoxLayout();
	pLytClose->addWidget(pLabClose);
	pLytClose->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_CLOSE]);

	QHBoxLayout* pLytOpen = new QHBoxLayout();
	pLytOpen->addWidget(pLabOpen);
	pLytOpen->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_OPEN]);

	QHBoxLayout* pLytHigh = new QHBoxLayout();
	pLytHigh->addWidget(pLabHigh);
	pLytHigh->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_HIGH]);

	QHBoxLayout* pLytLow = new QHBoxLayout();
	pLytLow->addWidget(pLabLow);
	pLytLow->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_LOW]);

	QHBoxLayout* pLytMA10 = new QHBoxLayout();
	pLytMA10->addWidget(pLabMA10);
	pLytMA10->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_MA10]);

	QHBoxLayout* pLytRSI7 = new QHBoxLayout();
	pLytRSI7->addWidget(pLabRSI7);
	pLytRSI7->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_RSI7]);

	QHBoxLayout* pLytData = new QHBoxLayout();
	pLytData->addWidget(pLabData);
	pLytData->addWidget(m_pLabCode[STOCK_REAL_WIDGET_INDEX_DATE]);

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
	pLytMain->addLayout(pLytData);
	pLytMain->setMargin(0);
	pWidget->setLayout(pLytMain);
	pWidget->setObjectName("Stock_Real_Widget");
}

void CStockRealWidget::OnInitTraceWidget(QWidget* pWidget)
{
	QVBoxLayout* pLytMain = new QVBoxLayout();
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	for (int i = 0; i < STOCK_REAL_WIDGET_TRACE_INDEX_NUM; i++)
	{
		m_pTraceLabCode[i] = new QLabel();
		m_pTraceLabCode[i]->setText(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_TEXT_DFT));
		m_pTraceLabCode[i]->setObjectName("Stock_Real_Lab_Val");
	}

	QHBoxLayout* pLytLab = new QHBoxLayout();
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_CODE]);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_NAME]);
	pLytLab->setAlignment(Qt::AlignCenter);
	m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_CODE]->setObjectName("Stock_Real_Lab_Code");
	m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_NAME]->setObjectName("Stock_Real_Lab_Code");
	pLytMain->addLayout(pLytLab);

	QLabel* pSeperator = new QLabel();
	pSeperator->setObjectName("Stock_Real_Lab_Sep");
	pLytMain->addWidget(pSeperator);

	pLytLab = new QHBoxLayout();
	QLabel* pLabTitle = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_NAME_HIGHTIME));
	
	pLytLab->addWidget(pLabTitle);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_HIGHTIME]);
	pLytMain->addLayout(pLytLab);

	pLytLab = new QHBoxLayout();
	pLabTitle = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_NAME_HIGHVAL));

	pLytLab->addWidget(pLabTitle);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_HIGHVAL]);
	pLytMain->addLayout(pLytLab);

	pLytLab = new QHBoxLayout();
	pLabTitle = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_NAME_BUYTIME));

	pLytLab->addWidget(pLabTitle);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_BUYTIME]);
	pLytMain->addLayout(pLytLab);

	pLytLab = new QHBoxLayout();
	pLabTitle = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_NAME_BUYVAL));

	pLytLab->addWidget(pLabTitle);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_BUYVAL]);
	pLytMain->addLayout(pLytLab);

	pLytLab = new QHBoxLayout();
	pLabTitle = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_NAME_SELLTIME));

	pLytLab->addWidget(pLabTitle);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_SELLTIME]);
	pLytMain->addLayout(pLytLab);

	pLytLab = new QHBoxLayout();
	pLabTitle = new QLabel(pCodec->toUnicode(STOCK_REAL_WIDGET_TRACE_NAME_SELLVAL));

	pLytLab->addWidget(pLabTitle);
	pLytLab->addWidget(m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_SELLVAL]);
	pLytMain->addLayout(pLytLab);
	pLytMain->setMargin(0);
	pWidget->setLayout(pLytMain);
	pWidget->setObjectName("Stock_Real_Widget");
}

void CStockRealWidget::Retranslate()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnFreshTimeout()));
	connect(pStockAgent, SIGNAL(NotifyUiRealKLineResponese()), this, SLOT(OnUpdateStockRealKLine()));
	connect(this, SIGNAL(QueryStockRealKLine(QString& ,QString&)), pStockAgent, SLOT(OnGetQueryRealKLine(QString&, QString&)));
}

void CStockRealWidget::OnSelectStock(QString& code, QString& name)
{
	m_curStockCode = code;
	m_curStockName = name;
}

void CStockRealWidget::OnFreshTimeout()
{
	if (m_curStockCode.isEmpty())
		return;

	emit  QueryStockRealKLine(m_curStockCode, m_curStockName);
}

void CStockRealWidget::OnUpdateStockRealKLine()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	QTextCodec* pCodec = QTextCodec::codecForLocale();

	QT_STOCK_REALKLINE_INFO realKLineInfo;
	if (!pStockAgent->GetStockRealKLine(&realKLineInfo))
	{
		for (int i = 0; i < STOCK_REAL_WIDGET_INDEX_NUM; i++)
		{
			m_pLabCode[i] = new QLabel();
			m_pLabCode[i]->setText(pCodec->toUnicode(STOCK_REAL_WIDGET_TEXT_DFT));
		}
	}

	m_pLabCode[STOCK_REAL_WIDGET_INDEX_CODE]->setText(pCodec->toUnicode(realKLineInfo.code));
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_NAME]->setText(pCodec->toUnicode(realKLineInfo.name));

	QString close = QString::number(realKLineInfo.realKLine.fClose, 'g', 4);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_CLOSE]->setText(close);

	QString open = QString::number(realKLineInfo.realKLine.fOpen, 'g', 4);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_OPEN]->setText(open);

	QString high = QString::number(realKLineInfo.realKLine.fHigh, 'g', 4);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_HIGH]->setText(high);

	QString low = QString::number(realKLineInfo.realKLine.fLow, 'g', 4);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_LOW]->setText(low);

	QString ma10 = QString::number(realKLineInfo.realKLine.fMa10, 'g', 4);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_MA10]->setText(ma10);

	QString rsi7 = QString::number(realKLineInfo.realKLine.fRsi7, 'g', 4);
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_RSI7]->setText(rsi7);

	QDateTime dateVal = QDateTime::fromTime_t(realKLineInfo.realKLine.timeVal);
	QString date = dateVal.toString("yyyy-MM-d");
	m_pLabCode[STOCK_REAL_WIDGET_INDEX_DATE]->setText(date);
}