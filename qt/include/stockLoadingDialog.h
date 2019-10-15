/*
*stockLoadingDialog.h
*/

#ifndef __STOCK_LOADING_DIALOG_H__
#define	__STOCK_LOADING_DIALOG_H__

#include <qwidget.h>
#include <qdialog.h>

class QLabel;
struct QT_STOCK_LOADING_MANAGER;
class CStockLoadingDialog :public QDialog 
{
	Q_OBJECT
public:
	CStockLoadingDialog(QWidget* parent);
	~CStockLoadingDialog();

private:
	void Onit();
	void RetranslateUi();

protected slots:
	void OnUpdateLoadingProgress(QString& stat, QString& progress);

private:
	QLabel*		m_pLabStat;
	QLabel*		m_pLabProgress;
};
#endif // !__STOCK_LOADING_DIALOG_H__
