/*queryTraceRecordDialog.cpp
*/

#include <qtreewidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qheaderview.h>
#include <qpushbutton.h>
#include <qtextcodec.h>
#include <qlayout.h>
#include <qdatetimeedit.h>
#include <qmessagebox.h>
#include "../include/qtWaittingDialog.h"
#include "../include/qtStockAgent.h"
#include "../include/qtResourceDef.h"
#include "../include/qtStockTraceDef.h"
#include "../include/queryTraceRecordDialog.h"


CQueryTraceRecordDialog::CQueryTraceRecordDialog(QWidget* pParent, CQtObjectAgent* pStockAgent):QDialog(pParent)
{
	m_pWaitDialog = NULL;
	m_pStockAgent = pStockAgent;
	OnInit();
	RetranslateUi();
}

CQueryTraceRecordDialog::~CQueryTraceRecordDialog()
{

}

void CQueryTraceRecordDialog::OnInit()
{
	QDate curDate = QDate::currentDate();
	QTextCodec* pCodec = QTextCodec::codecForLocale();

	QLabel* pLab = new QLabel(pCodec->toUnicode(QUERY_TRACE_RECORD_DIALOG_LAB_STARTTIME));
	m_pDateEditStart = new QDateEdit();
	
	m_pDateEditStart->setMaximumDate(curDate.addDays(-1));
	m_pDateEditStart->setDisplayFormat("yyyy/MM/dd");

	QHBoxLayout* pLytStart = new QHBoxLayout();
	pLytStart->addWidget(pLab);
	pLytStart->addWidget(m_pDateEditStart);


	pLab = new QLabel(pCodec->toUnicode(QUERY_TRACE_RECORD_DIALOG_LAB_ENDTIME));
	m_pDateEditEnd = new QDateEdit();
	m_pDateEditEnd->setMaximumDate(curDate);
	m_pDateEditEnd->setDisplayFormat("yyyy/MM/dd");
	m_pDateEditEnd->setDate(curDate);
	QHBoxLayout* pLytEnd = new QHBoxLayout();
	pLytEnd->addWidget(pLab);
	pLytEnd->addWidget(m_pDateEditEnd);

	m_pBtnQuery = new QPushButton(pCodec->toUnicode(QUERY_TRACE_RECORD_DIALOG_BTN_QUERY));

	QHBoxLayout* pLytQuery = new QHBoxLayout();
	pLytQuery->addLayout(pLytStart);
	pLytQuery->addLayout(pLytEnd);
	pLytQuery->addWidget(m_pBtnQuery);

	m_pTreeRecord = new QTreeWidget();
	QStringList headerList;
	char const* header[] = {
		QUERY_TRACE_RECORD_DIALOG_TREE_CODE_NAME, QUERY_TRACE_RECORD_DIALOG_TREE_HIGHTIME_NAME, QUERY_TRACE_RECORD_DIALOG_TREE_HIGHVAL_NAME,
		QUERY_TRACE_RECORD_DIALOG_TREE_BUYTIME_NAME, QUERY_TRACE_RECORD_DIALOG_TREE_BUYVAL_NAME, QUERY_TRACE_RECORD_DIALOG_TREE_SELLTIME_NAME,
		QUERY_TRACE_RECORD_DIALOG_TREE_SELLVAL_NAME,QUERY_TRACE_RECORD_DIALOG_TREE_TOPTIME_NAME,QUERY_TRACE_RECORD_DIALOG_TREE_TOPVAL_NAME,
		QUERY_TRACE_RECORD_DIALOG_TREE_EARN_NAME
	};

	for (int i = 0; i < QUERY_TRACE_RECORD_DIALOG_TREE_NUM; i++)
	{
		headerList << pCodec->toUnicode(header[i]);
	}

	m_pTreeRecord->setHeaderLabels(headerList);

	m_pTreeRecord->setHeaderLabels(headerList);
	m_pTreeRecord->header()->setSectionResizeMode(QHeaderView::Stretch);
	m_pTreeRecord->header()->setStretchLastSection(false);
	m_pTreeRecord->header()->setDefaultAlignment(Qt::AlignCenter);
	m_pTreeRecord->setIndentation(0);
	QVBoxLayout* pLyt = new QVBoxLayout();

	pLyt->addLayout(pLytQuery);
	pLyt->addWidget(m_pTreeRecord);
	pLyt->setMargin(0);
	this->setLayout(pLyt);
}

void CQueryTraceRecordDialog::RetranslateUi()
{
	connect(m_pBtnQuery, SIGNAL(clicked(bool)), this, SLOT(OnQueryClicked(bool)));
	connect(this, SIGNAL(NotifyQueryTraceRecord(int, int, int)), m_pStockAgent, SLOT(OnGetQueryTraceRecord(int, int, int)));
	connect(m_pStockAgent, SIGNAL(NotifyUiTraceRecordResponse()), this, SLOT(OnNotifyQueyTraceRecorResponse()));
}

void CQueryTraceRecordDialog::UpdateRecordTree(QT_STOCK_TRACERECORD_INFO const* pRecordInfo)
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	QTreeWidgetItem* pItem = new QTreeWidgetItem();

	QString code = pCodec->toUnicode(pRecordInfo->code);
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_CODE_INDEX, code);

	QString highTime = QDateTime::fromTime_t(pRecordInfo->highTime).date().toString("yyyy/MM/dd");
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_HIGHTIME_INDEX, highTime);

	QString highVal = QString::number(pRecordInfo->fHighVal, 'g', 4);
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_HIGHVAL_INDEX, highVal);

	QString buyTime = QDateTime::fromTime_t(pRecordInfo->buyTime).date().toString("yyyy/MM/dd");
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_BUYTIME_INDEX, buyTime);

	QString buyVal = QString::number(pRecordInfo->fBuyVal, 'g', 4);
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_BUYVAL_INDEX, buyVal);

	QString sellTime = QDateTime::fromTime_t(pRecordInfo->sellTime).date().toString("yyyy/MM/dd");
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_SELLTIME_INDEX, sellTime);

	QString sellVal = QString::number(pRecordInfo->fSellVal, 'g', 4);
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_SELLVAL_INDEX, sellVal);

	QString topTime = QDateTime::fromTime_t(pRecordInfo->topTime).date().toString("yyyy/MM/dd");
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_TOPTIME_INDEX, topTime);

	QString topVal = QString::number(pRecordInfo->fTopVal, 'g', 4);
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_TOPVAL_INDEX, topVal);

	float fEarn = (pRecordInfo->fSellVal - pRecordInfo->fBuyVal) * 100 / pRecordInfo->fBuyVal;
	QString earn = QString::number(fEarn, 'g', 4);
	pItem->setText(QUERY_TRACE_RECORD_DIALOG_TREE_EARN_INDEX, earn);
	m_pTreeRecord->addTopLevelItem(pItem);
}

void CQueryTraceRecordDialog::ClearRecordTree()
{
	do
	{
		QTreeWidgetItem* pItem = m_pTreeRecord->takeTopLevelItem(0);
		if (pItem == NULL)
			break;

		delete pItem;
	} while (1);
}

void CQueryTraceRecordDialog::OnQueryClicked(bool check)
{
	QDate startDate = m_pDateEditStart->date();
	QDate endDate = m_pDateEditEnd->date();
	if (startDate > endDate)
	{
		QTextCodec* pCodec = QTextCodec::codecForLocale();
		QMessageBox::warning(this, pCodec->toUnicode(QUERY_TRACE_RECORD_WARNING_DATE_TITLE), pCodec->toUnicode(QUERY_TRACE_RECORD_WARNING_DATE_TEXT));
		return;
	}
	m_offsetRecord = 0;
	int startTime = QDateTime(startDate).toTime_t();
	int endTime = QDateTime(endDate.addDays(1)).toTime_t();
	ClearRecordTree();
	m_pBtnQuery->setEnabled(false);
	emit NotifyQueryTraceRecord(startTime, endTime, 0);
	m_pWaitDialog = new CQtWaitting(NULL);
	m_pWaitDialog->exec();
}

void CQueryTraceRecordDialog::OnNotifyQueyTraceRecorResponse()
{
	CQtStockAgent* pStockAgent = (CQtStockAgent*)m_pStockAgent;
	QT_STOCK_TRACERECORD_INFO recordInfo;
	int getCnts = 0;
	do
	{
		if (pStockAgent->GetStockTraceRecord(&recordInfo, getCnts, 1) <= 0)
			break;

		UpdateRecordTree(&recordInfo);
		getCnts++;
	} while (1);


	if (getCnts > 0)
	{
		QDate startDate = m_pDateEditStart->date();
		QDate endDate = m_pDateEditEnd->date();

		m_offsetRecord += getCnts;
		int startTime = QDateTime(startDate).toTime_t();
		int endTime = QDateTime(endDate.addDays(1)).toTime_t();
		emit NotifyQueryTraceRecord(startTime, endTime, m_offsetRecord);
	}
	else
	{
		m_pWaitDialog->close();
		delete m_pWaitDialog;
		m_pBtnQuery->setEnabled(true);
	}
}