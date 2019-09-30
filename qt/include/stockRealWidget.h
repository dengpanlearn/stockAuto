/*
*stockTraceWidget.h
*/

#ifndef __STOCK_REAL_WIDGET_H__
#define	__STOCK_REAL_WIDGET_H__


#include <QWidget>

class CQtObjectAgent;
class QText;

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
	QText*		m_pTextCode;
	QText*		m_pTextName;
};

#endif // !__STOCK_REAL_WIDGET_H__