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
#include "../include/qtResourceDef.h"
#include "../include/queryTraceRecordDialog.h"


CQueryTraceRecordDialog::CQueryTraceRecordDialog(QWidget* pParent)
{
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

}