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

class CStockRealWidget : public QWidget
{
	Q_OBJECT

public:
	CStockRealWidget(QWidget *parent);
	~CStockRealWidget();

private:
	void OnInit();
	void Retranslate();

private:
	CQtObjectAgent*		m_pExitAgent;
	CQtObjectAgent*		m_pStockAgent;
	QLabel*		m_pLabCode[STOCK_REAL_WIDGET_IDEX_NUM];

};

#endif // !__STOCK_REAL_WIDGET_H__