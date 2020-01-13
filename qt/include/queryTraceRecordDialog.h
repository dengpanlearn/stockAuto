/*queryTraceRecordDialog.h
*/

#ifndef __QUERY_TRACE_RECORD_DIALOG_H__
#define __QUERY_TRACE_RECORD_DIALOG_H__


#include <qdialog.h>
class QPushButton;
class QTreeWidget;
class QDateEdit;
class CQtWaitting;
class CQtObjectAgent;

struct QT_STOCK_TRACERECORD_INFO;
class CQueryTraceRecordDialog :public QDialog
{
	Q_OBJECT
public:
	CQueryTraceRecordDialog(QWidget* pParent , CQtObjectAgent* pStockAgent);
	~CQueryTraceRecordDialog();

private:
	void OnInit();
	void RetranslateUi();
	void UpdateRecordTree(QT_STOCK_TRACERECORD_INFO const* pRecordInfo);
	void ClearRecordTree();

signals:
	void NotifyQueryTraceRecord(int startTime, int endTime, int offset);

private slots:
	void OnQueryClicked(bool check);
	void OnNotifyQueyTraceRecorResponse();

private:
	QPushButton*	m_pBtnQuery;
	QTreeWidget*	m_pTreeRecord;
	QDateEdit*		m_pDateEditStart;
	QDateEdit*		m_pDateEditEnd;
	CQtWaitting*	m_pWaitDialog;
	CQtObjectAgent* m_pStockAgent;
	int				m_offsetRecord;
};
#endif // !__QUERY_TRACE_RECORD_DIALOG_H__
