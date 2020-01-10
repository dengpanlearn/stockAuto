/*queryTraceRecordDialog.h
*/

#ifndef __QUERY_TRACE_RECORD_DIALOG_H__
#define __QUERY_TRACE_RECORD_DIALOG_H__


#include <qdialog.h>
class QPushButton;
class QTreeWidget;
class QDateEdit;
class CQueryTraceRecordDialog :public QDialog
{
	Q_OBJECT
public:
	CQueryTraceRecordDialog(QWidget* pParent = NULL);
	~CQueryTraceRecordDialog();

private:
	void OnInit();
	void RetranslateUi();

private:
	QPushButton*	m_pBtnQuery;
	QTreeWidget*	m_pTreeRecord;
	QDateEdit*		m_pDateEditStart;
	QDateEdit*		m_pDateEditEnd;
};
#endif // !__QUERY_TRACE_RECORD_DIALOG_H__
