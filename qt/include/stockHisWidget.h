/*
*stockTraceWidget.h
*/

#ifndef __STOCK_HIS_WIDGET_H__
#define	__STOCK_HIS_WIDGET_H__

#include <dp.h>
#include <QWidget>
#include <stockCalcDef.h>

class QTreeWidgetItem;
class QTreeWidget;
class QPushButton;
class QLineEdit;
class CQtObjectAgent;
class CQtWaitting;

class CStockHisWidget : public QWidget
{
	Q_OBJECT

public:
	CStockHisWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent);
	~CStockHisWidget();



signals:
	void QueryStockHisKLine(QString& code);
private:
	void OnInit();
	void Retranslate();

private:
	static void ShowHisKLineItem(QTreeWidgetItem* pItem, STOCK_CALC_TRACE_KLINE* pKLine);
	void ShowHisKLine(int offset, int counts, STOCK_CALC_TRACE_KLINE* pKLine);
	void DeleteUnusedKLineItem(int usedCounts);
	

private slots:
	void OnHisKLineQueryClick(bool);
	void OnHisKLineQueryResponse();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;
	QPushButton*		m_pBtnSearch;
	QLineEdit*			m_pEditCodeName;
	QTreeWidget*		m_pTreeHisKLine;
	CQtWaitting*		m_pWaittingDialog;
	STOCK_CALC_TRACE_KLINE m_klineBuf[10];

};

#endif // !__STOCK_HIS_WIDGET_H__