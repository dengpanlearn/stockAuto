/*
*stockTraceWidget.h
*/

#ifndef __STOCK_HIS_WIDGET_H__
#define	__STOCK_HIS_WIDGET_H__


#include <QWidget>

class CQtObjectAgent;


class CStockHisWidget : public QWidget
{
	Q_OBJECT

public:
	CStockHisWidget(QObject *parent);
	~CStockHisWidget();

private:
	void OnInit();
	void Retranslate();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;

};

#endif // !__STOCK_HIS_WIDGET_H__