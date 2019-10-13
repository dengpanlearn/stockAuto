
#include <qtextcodec.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qheaderview.h>
#include <qtreewidget.h>
#include <qmessagebox.h>
#include "../include/qtStockAgent.h"
#include "../include/qtResourceDef.h"
#include "../include/stockHisWidget.h"

CStockHisWidget::CStockHisWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent)
	: QWidget(parent)
{
	m_pExitAgent = pExitAgent;
	m_pStockAgent = pStockAgent;
	OnInit();
	Retranslate();
}

CStockHisWidget::~CStockHisWidget()
{
}


void CStockHisWidget::OnInit()
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	m_pEditCodeName = new QLineEdit();
	m_pBtnSearch = new QPushButton(pCodec->toUnicode(STOCK_HIS_WIDGET_BTN_SEARCH));

	QHBoxLayout* pLytCode = new QHBoxLayout();
	pLytCode->addWidget(m_pEditCodeName);
	pLytCode->addWidget(m_pBtnSearch);

	m_pTreeHisKLine = new QTreeWidget();

	QStringList headerList;
	char const* header[] = {
		STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_DATE, STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_HIGH, STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_LOW,
		STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_CLOSE, STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_OPEN, STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_MA10,
		STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_RSI
	};

	for (int i = 0; i < STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_NUM; i++)
	{
		headerList << pCodec->toUnicode(header[i]);
	}

	m_pTreeHisKLine->setHeaderLabels(headerList);
	m_pTreeHisKLine->header()->setSectionResizeMode(QHeaderView::QHeaderView::Stretch);
	m_pTreeHisKLine->header()->setStretchLastSection(false);
	m_pTreeHisKLine->header()->setDefaultAlignment(Qt::AlignCenter);
	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->setMargin(0);
	pLyt->addLayout(pLytCode);
	pLyt->addWidget(m_pTreeHisKLine);
	this->setLayout(pLyt);
}

void CStockHisWidget::Retranslate()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	connect(pStockAgent, SIGNAL(NotifyUiHisKLineResponese()), this, SLOT(OnHisKLineQueryResponse()));
	connect(m_pBtnSearch, SIGNAL(clicked(bool)), this, SLOT(OnHisKLineQueryClick(bool)));
	connect(this, SIGNAL(QueryStockHisKLine(QString&)), pStockAgent, SLOT(OnGetQueryHisKLine(QString&)));
}

void CStockHisWidget::OnHisKLineQueryResponse()
{

}

void CStockHisWidget::OnHisKLineQueryClick(bool check)
{
	QString codeInput = m_pEditCodeName->text();
	QString code = codeInput.toUpper();
	
	if (!code.startsWith("SH") && !code.startsWith("SZ"))
	{
		QMessageBox msgBox;
		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("输入有效股票代码"));

		msgBox.setIcon(QMessageBox::Information);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.exec();
	}
	else 
	{
		emit QueryStockHisKLine(code);
	}
}