/*
*stockTraceWidget.h
*/

#ifndef __STOCK_REAL_WIDGET_H__
#define	__STOCK_REAL_WIDGET_H__


#include <QWidget>

class CQtObjectAgent;


class CStockRealWidget : public QWidget
{
	Q_OBJECT

public:
	CStockRealWidget(QObject *parent);
	~CStockRealWidget();

private:
	void OnInit();
	void Retranslate();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;

};

#endif // !__STOCK_REAL_WIDGET_H__