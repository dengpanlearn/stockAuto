/*
*stockTraceWidget.h
*/

#ifndef __STOCK_TRACE_WIDGET_H__
#define	__STOCK_TRACE_WIDGET_H__


#include <QWidget>

class CQtObjectAgent;


class CStockTraceWidget : public QWidget
{
	Q_OBJECT

public:
	CStockTraceWidget(QObject *parent);
	~CStockTraceWidget();

private:
	void OnInit();
	void Retranslate();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;

};

#endif // !__STOCK_TRACE_WIDGET_H__