/*
*stockLoadingWidget.cpp
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qtextcodec.h>

#include "../include/qtResourceDef.h"
#include "../include/stockLoadingDialog.h"


CStockLoadingDialog::CStockLoadingDialog(QWidget* parent)
{
	Onit();
	RetranslateUi();
}

CStockLoadingDialog::~CStockLoadingDialog()
{

}

void CStockLoadingDialog::Onit()
{
	QTextCodec* pCodec = QTextCodec::codecForLocale();
	m_pLabStat = new QLabel(pCodec->toUnicode(STOCK_LOADING_DIALOG_LAB_STAT_INIT));

	m_pLabProgress = new QLabel(QString::number(0));
	m_pLabProgress->setVisible(false);
	m_pLabProgress->setVisible(false);
	QHBoxLayout* pLyt = new QHBoxLayout();
	pLyt->addWidget(m_pLabStat);
	pLyt->addWidget(m_pLabProgress);
	this->setLayout(pLyt);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
}

void CStockLoadingDialog::RetranslateUi()
{

}

void CStockLoadingDialog::OnUpdateLoadingProgress(QString& stat, QString& progress)
{
	m_pLabStat->setText(stat);
	if (progress.isEmpty())
		m_pLabProgress->setVisible(false);
	else
	{
		m_pLabProgress->setVisible(true);
		m_pLabProgress->setText(progress);
	}
}
