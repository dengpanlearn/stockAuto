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
class QTreeWidgetItem;

class CStockTraceWidget : public QWidget
{
	Q_OBJECT

public:
	CStockTraceWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent);
	~CStockTraceWidget();

private:
	void OnInit();
	void Retranslate();

signals:
	void SignalSelectStock(QString& code, QString& name);

private slots:
	void OnNotifyStockTrace();
	void OnSelectStock(QTreeWidgetItem * pItem, int column);

private:
	void AddTraceLogItem(char const* pCode, char const* pName, UINT stat);
	void ModifyTraceLogItem(char const* pCode, char const* pName, UINT stat);
	void DeleteTraceLogItem(char const* pCode, char const* pName);

	QTreeWidgetItem* FindTraceLogItem(QTreeWidget* pTreeWidget, char const* pCode, char const* pName);

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;

	QTreeWidget*		m_pTreeWaitBuy;
	QTreeWidget*		m_pTreeWaitSell;
	QWidget*			m_pWidgetWaitBuy;
	QWidget*			m_pWidgetWaitSell;
	QSplitter*			m_pSplitter;
};

#endif // !__STOCK_TRACE_WIDGET_H__