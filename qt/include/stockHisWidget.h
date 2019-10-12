/*
*stockTraceWidget.h
*/

#ifndef __STOCK_HIS_WIDGET_H__
#define	__STOCK_HIS_WIDGET_H__

#include <dp.h>
#include <QWidget>

class QTreeWidget;
class QPushButton;
class QLineEdit;
class CQtObjectAgent;


class CStockHisWidget : public QWidget
{
	Q_OBJECT

public:
	CStockHisWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent);
	~CStockHisWidget();

private:
	void OnInit();
	void Retranslate();

private slots:
	void OnHisKLineQueryClick(bool);
	void OnHisKLineQueryResponse();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;
	QPushButton*		m_pBtnSearch;
	QLineEdit*			m_pEditCodeName;
	QTreeWidget*		m_pTreeHisKLine;

};

#endif // !__STOCK_HIS_WIDGET_H__