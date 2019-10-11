/*
*stockTraceWidget.h
*/

#ifndef __STOCK_TRACE_WIDGET_H__
#define	__STOCK_TRACE_WIDGET_H__

#include <dp.h>
#include <QWidget>

class CQtObjectAgent;
class QTreeWidget;
class QWidget;
class QSplitter;

class CStockTraceWidget : public QWidget
{
	Q_OBJECT

public:
	CStockTraceWidget(QWidget *parent);
	~CStockTraceWidget();

private:
	void OnInit();
	void Retranslate();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;

	QTreeWidget*		m_pTreeWaitBuy;
	QTreeWidget*		m_pTreeWaitSell;
	QWidget*			m_pWidgetWaitBuy;
	QWidget*			m_pWidgetWaitSell;
	QSplitter*				m_pSplitter;
};

#endif // !__STOCK_TRACE_WIDGET_H__