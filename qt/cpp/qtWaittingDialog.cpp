/*
*qtWaittingDialog.cpp
*/

#include <qmovie.h>
#include <qlabel.h>
#include <qlayout.h>
#include "../include/qtWaittingDialog.h"



CQtWaitting::CQtWaitting(QWidget* parent):QDialog(parent)
{
	Onit();
	RetranslateUi();
}

void CQtWaitting::Onit()
{
	m_pLabWait = new QLabel(NULL);
	m_pMovieWait = new QMovie(":/CStockAutoWindow/waitting.gif");
	m_pLabWait->setMovie(m_pMovieWait);
	setWindowOpacity(0.5);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint |Qt::FramelessWindowHint);
	setAutoFillBackground(true);

	QVBoxLayout* pLyt = new QVBoxLayout();
	pLyt->addWidget(m_pLabWait);
	this->setLayout(pLyt);
}

void CQtWaitting::RetranslateUi()
{
	m_pMovieWait->start();
}