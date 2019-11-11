
#include <qtextcodec.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qheaderview.h>
#include <qtreewidget.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qmetatype.h>
#include "../include/qtWaittingDialog.h"
#include "../include/qtStockAgent.h"
#include "../include/qtResourceDef.h"
#include "../include/stockHisWidget.h"

CStockHisWidget::CStockHisWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent)
	: QWidget(parent)
{
	m_pExitAgent = pExitAgent;
	m_pStockAgent = pStockAgent;
	m_pWaittingDialog = NULL;
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
		STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_RSI,STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_PERCENT
	};

	for (int i = 0; i < STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_NUM; i++)
	{
		headerList << pCodec->toUnicode(header[i]);
	}

	m_pTreeHisKLine->setHeaderLabels(headerList);
	m_pTreeHisKLine->header()->setSectionResizeMode(QHeaderView::Stretch);
	m_pTreeHisKLine->header()->setStretchLastSection(false);
	m_pTreeHisKLine->header()->setDefaultAlignment(Qt::AlignCenter);
	m_pTreeHisKLine->setIndentation(0);
	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->setMargin(0);
	pLyt->addLayout(pLytCode);
	pLyt->addWidget(m_pTreeHisKLine);
	this->setLayout(pLyt);
}

void CStockHisWidget::Retranslate()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	qRegisterMetaType<QString>("QString&");
	connect(pStockAgent, SIGNAL(NotifyUiHisKLineResponese()), this, SLOT(OnHisKLineQueryResponse()));
	connect(m_pBtnSearch, SIGNAL(clicked(bool)), this, SLOT(OnHisKLineQueryClick(bool)));
	connect(this, SIGNAL(QueryStockHisKLine(QString&)), pStockAgent, SLOT(OnGetQueryHisKLine(QString&)));
}

void CStockHisWidget::ShowHisKLineItem(QTreeWidgetItem* pItem, STOCK_CALC_TRACE_KLINE* pKLine)
{
	QDateTime dateVal = QDateTime::fromTime_t(pKLine->timeVal);
	QString date = dateVal.toString("yyyy-MM-d");
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_DATE, date);

	QString highVal = QString::number(pKLine->fHigh, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_HIGH, highVal);

	QString lowVal = QString::number(pKLine->fLow, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_LOW, lowVal);

	QString closeVal = QString::number(pKLine->fClose, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_CLOSE, closeVal);

	QString openVal = QString::number(pKLine->fOpen, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_OPEN, openVal);

	QString ma10Val = QString::number(pKLine->fMa10, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_MA10, ma10Val);

	QString rsiVal = QString::number(pKLine->fRsi7, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_RSI, rsiVal);


	QString perVal = QString::number(pKLine->fPercent, 'g', 4);
	pItem->setText(STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_PERCENT, perVal);
}

void CStockHisWidget::ShowHisKLine(int offset, int counts, STOCK_CALC_TRACE_KLINE* pKLine)
{
	int itemCounts = m_pTreeHisKLine->topLevelItemCount();
	int availItemCounts = itemCounts - offset;

	int newItemCounts = counts - availItemCounts;
	if (newItemCounts < 0)
		newItemCounts = 0;

	for (int i = 0; i < availItemCounts; i++, pKLine++)
	{
		QTreeWidgetItem* pTopItem = m_pTreeHisKLine->topLevelItem(offset++);

		ShowHisKLineItem(pTopItem, pKLine);
	}

	for (int i = 0; i < newItemCounts; i++, pKLine++)
	{
		QTreeWidgetItem* pTopItem = new QTreeWidgetItem();
		ShowHisKLineItem(pTopItem, pKLine);

		m_pTreeHisKLine->addTopLevelItem(pTopItem);
	}
}

void CStockHisWidget::DeleteUnusedKLineItem(int usedCounts)
{
	int itemCounts = m_pTreeHisKLine->topLevelItemCount();
	
	int unusedCounts = itemCounts - usedCounts;
	int offset = usedCounts;
	for (int i = 0; i < unusedCounts; i++, offset++)
	{
		QTreeWidgetItem* pTopItem = m_pTreeHisKLine->takeTopLevelItem(offset);
		delete pTopItem;
	}
}

void CStockHisWidget::OnHisKLineQueryResponse()
{
	int ret = 0;
	int offset = 0;
	int bufCounts = sizeof(m_klineBuf) / sizeof(m_klineBuf[0]);
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;

	do
	{
		ret = pStockAgent->GetStockHisKLine(bufCounts, offset, m_klineBuf);
		if (ret <= 0)
			break;
		
		ShowHisKLine(offset, bufCounts, m_klineBuf);
		offset += ret;
	} while (1);


	DeleteUnusedKLineItem(offset);
	if (m_pWaittingDialog != NULL)
	{
		m_pWaittingDialog->close();
		delete m_pWaittingDialog;
		m_pWaittingDialog = NULL;
	}

	if (ret < 0)
	{
		QMessageBox msgBox;
		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("输入有效股票代码"));

		msgBox.setIcon(QMessageBox::Information);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.exec();
	}
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

		m_pWaittingDialog = new CQtWaitting(this);
		m_pWaittingDialog->setObjectName("Waitting_Dialog");
		emit QueryStockHisKLine(code);
		
		m_pWaittingDialog->exec();
	}
}