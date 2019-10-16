/*
*stockAutoWindow.h
*/

#ifndef __STOCK_AUTO_WINDOW_H__
#define	__STOCK_AUTO_WINDOW_H__

#include <dp.h>
#include <QtWidgets/QMainWindow>
#include "stockHisWidget.h"
#include "stockRealWidget.h"
#include "stockTraceWidget.h"
#include "stockLoadingDialog.h"
#include "qtServerTask.h"
#include "qtStockTraceDef.h"
#include "qtObjectAgent.h"
#include "qtStockAgent.h"

class CStockAutoWindow : public QMainWindow
{
    Q_OBJECT

public:
    CStockAutoWindow(QWidget *parent = Q_NULLPTR);
	CStockAutoWindow::~CStockAutoWindow();


public:
	void UpdateAutoManagerStep(UINT traceStep, int loadProgress);

private:
	void OnInit();
	void RetranlateUi();

	BOOL OnInitQtServerAndAgent();

signals:
	void UpdateLoadingProgress(QString& stat, QString& progress);

protected:
	virtual void closeEvent(QCloseEvent * event);

private slots:
	void OnNotifyAutoManagerLoadingProgress();
	void OnNotifyStockTrace();

private:
	CQtServerTask*	m_pServerTask;
	CQtObjectAgent* m_pExitAgent;
	CQtObjectAgent*	m_pStockAgent;
	CStockTraceWidget*	m_pTraceWidget;
	CStockRealWidget*	m_pRealWidget;
	CStockHisWidget*	m_pHisWidget;
	CStockLoadingDialog*	m_pLoadingDialog;
	BOOL		m_bLoading;
};

#endif // !__STOCK_AUTO_WINDOW_H__
