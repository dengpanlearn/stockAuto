/*
*stockTraceWidget.h
*/

#ifndef __STOCK_REAL_WIDGET_H__
#define	__STOCK_REAL_WIDGET_H__

#include <dp.h>
#include <QWidget>
#include "qtResourceDef.h"

class CQtObjectAgent;
class QLabel;
class QTimer;
class QSplitter;

class CStockRealWidget : public QWidget
{
	Q_OBJECT

public:
	CStockRealWidget(QWidget *parent, CQtObjectAgent* pExitAgent, CQtObjectAgent* pStockAgent);
	~CStockRealWidget();

private:
	void OnInit();
	void OnInitRealWidget(QWidget* pWidget);
	void OnInitTraceWidget(QWidget* pWidget);
	void Retranslate();

signals:
	void QueryStockRealKLine(QString& code, QString& name);
	void QueryStockTraceInfo(QString& code, QString& name);

private slots:
	void OnSelectStock(QString& code, QString& name);
	void OnFreshTimeout();
	void OnUpdateStockRealKLine();
	void OnUpdateStockTraceInfo();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;
	QLabel*		m_pLabCode[STOCK_REAL_WIDGET_INDEX_NUM];
	QLabel*		m_pTraceLabCode[STOCK_REAL_WIDGET_TRACE_INDEX_NUM];
	QString		m_curStockCode;
	QString		m_curStockName;
	QTimer*		m_pTimer;
	QSplitter*	m_pSpliter;
};

#endif // !__STOCK_REAL_WIDGET_H__